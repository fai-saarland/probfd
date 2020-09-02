#include "abstraction.h"

#include "label_reducer.h"
// #include "merge_and_shrink_heuristic.h" // needed for ShrinkStrategy type;
// TODO: move that type somewhere else?
#include "shrink_fh.h"

#include "../globals.h"
#include "../global_operator.h"
#include "../option_parser.h" // TODO: Should be removable later.
#include "../utils/timer.h"
#include "../utils/system.h"
#include "utils.h"
#include "../algorithms/priority_queues.h"






#include <fstream>



#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <set>
using namespace std;

namespace merge_and_shrink {


/* Implementation note: Transitions are grouped by their operators,
 not by source state or any such thing. Such a grouping is beneficial
 for fast generation of products because we can iterate operator by
 operator, and it also allows applying abstraction mappings very
 efficiently.

 We rarely need to be able to efficiently query the successors of a
 given state; actually, only the distance computation requires that,
 and it simply generates such a graph representation of the
 transitions itself. Various experiments have shown that maintaining
 a graph representation permanently for the benefit of distance
 computation is not worth the overhead.
 */

/*
 TODO:

 * Separate out all code related to shrinking strategies into a
 separate shrinking strategy class.

 * Try out shrinking strategies that don't care about g.

 * NOTE: We might actually also try out DFP-style strategies that don't
 just consider h, but also g. Essentially, we might set it up in such
 a way that it works with arbitrary bucketings and bucket priority
 orderings -- we'd just need to replace the current "h" components
 by bucket indices.

 * Only compute g and h when you actually need to (i.e., don't compute
 g if the strategies doesn't need it; don't compute h for the random
 shrinking strategy except for the final abstraction where we need
 it for the h value). Actually... we do need to compute g values anyway
 for the relevance test. So never mind. Although we could get rid of
 the recomputation for non-f-preserving abstractions, but that's true
 for all strategies because none of them actually needs that.

 * It's maybe a bit of a problem that by indexing the transitions
 with the *global* operator no, we have overall size requirements
 of NUM_OPS * NUM_STATE_VARS for all abstractions together. This
 is all the more unfortunate considering that we only really need
 two abstractions at the same time. Can we do something about this?

 Idea: Use an alternative indexing scheme, e.g. ones where the vector
 is indexed by some sort of "local" operator id. If we don't need
 to the lookup frequently (and I don't think we do), then we can
 use a simple std::unordered_map<GlobalOperator *, int> for this purpose.

 * Separate the parts relating to the abstraction mapping from the
 actual abstraction, so that later during search we need not keep
 the abstractions. This would allow us to get rid of the somewhat
 ugly "release_memory" stuff.

 * Change all the terminology here so that it matches the one in
 recent papers ("merge-and-shrink", "abstraction", "transition
 system", "atomic abstraction", "product abstraction", etc.)
 Currently, "abstraction" is used both for the abstract transition
 system and for the abstraction mapping. It should only be used for the
 latter; the former maybe called "TransitionSystem" or whatever.

 */

//  TODO: We define infinity in more than a few places right now (=>
//        grep for it). It should only be defined once.
static const int infinity = numeric_limits<int>::max();

const int Abstraction::PRUNED_STATE = -1;
const int Abstraction::DISTANCE_UNKNOWN = -2;


inline int get_op_index(const GlobalOperator *op)
{
    /* TODO: The op_index computation is duplicated from
     LabelReducer::get_op_index() and actually belongs neither
     here nor there. There should be some canonical way of getting
     from an GlobalOperator pointer to an index, but it's not clear how to
     do this in a way that best fits the overall planner
     architecture (taking into account axioms etc.) */
    int op_index = op - &*g_operators.begin();
    assert(op_index >= 0 && (size_t) op_index < g_operators.size());
    return op_index;
}

Abstraction::Abstraction(bool is_unit_cost_, OperatorCost cost_type_)
    : is_unit_cost(is_unit_cost_), cost_type(cost_type_), irrelevant(false),
      are_labels_reduced(false), peak_memory(0), max_num_states(0),
      max_num_transitions(0), overall_peak_memory(0), overall_max_num_states(0),
      overall_max_num_transitions(0), all_goal_vars_in(false),
      last_computation_used_uniform_costs(false)
{
    clear_distances();
    transitions_by_op.resize(g_operators.size());
}

Abstraction::~Abstraction()
{
}

string Abstraction::tag() const
{
    string desc(description());
    desc[0] = toupper(desc[0]);
    return desc + ": ";
}

void Abstraction::clear_distances()
{
    max_f = DISTANCE_UNKNOWN;
    max_g = DISTANCE_UNKNOWN;
    max_h = DISTANCE_UNKNOWN;
    init_distances.clear();
    goal_distances.clear();
}

int Abstraction::size() const
{
    return num_states;
}

int Abstraction::get_max_f() const
{
    return max_f;
}

int Abstraction::get_max_g() const
{
    return max_g;
}

int Abstraction::get_max_h() const
{
    return max_h;
}

void Abstraction::compute_distances(bool prune_unreachable,
                                    bool prune_irrelevant, bool continue_if_unsolvable, bool use_uniform_costs,
                                    bool force_recomputation)
{
    cout << tag() << flush;
    if (force_recomputation) {
        clear_distances();
    }
    if (max_h != DISTANCE_UNKNOWN) {
        cout << "distances already known" << endl;
        return;
    }
    last_computation_used_uniform_costs = use_uniform_costs;

    assert(init_distances.empty() && goal_distances.empty());

    if (init_state == PRUNED_STATE && !continue_if_unsolvable) {
        cout << "init state was pruned, no distances to compute" << endl;
        // If init_state was pruned, then everything must have been pruned.
        assert(num_states == 0);
        max_f = max_g = max_h = infinity;
        return;
    }

    init_distances.resize(num_states,
                          prune_unreachable ? infinity : infinity - 1);
    goal_distances.resize(num_states,
                          prune_irrelevant ? infinity : infinity - 1);
    if (is_unit_cost || use_uniform_costs) {
        cout << "computing distances using unit-cost algorithm" << endl;
        compute_init_distances_unit_cost();
        compute_goal_distances_unit_cost();
    } else {
        cout << "computing distances using general-cost algorithm" << endl;
        compute_init_distances_general_cost();
        compute_goal_distances_general_cost();
    }

    max_f = 0;
    max_g = 0;
    max_h = 0;

    if (prune_unreachable || prune_irrelevant) {
        int unreachable_count = 0, irrelevant_count = 0;
        for (int i = 0; i < num_states; i++) {
            int g = init_distances[i];
            int h = goal_distances[i];
            // GlobalStates that are both unreachable and irrelevant are counted
            // as unreachable, not irrelevant. (Doesn't really matter, of
            // course.)
            if (g == infinity) {
                unreachable_count++;
            } else if (h == infinity) {
                irrelevant_count++;
            } else {
                if (g == infinity - 1) {
                    max_f = infinity - 1;
                } else if (h == infinity - 1) {
                    max_f = infinity - 1;
                } else {
                    max_f = max(max_f, g + h);
                }
                max_g = max(max_g, g);
                max_h = max(max_h, h);
            }
        }
        if (unreachable_count || irrelevant_count) {
            cout << tag()
                 << "unreachable: " << unreachable_count << " states, "
                 << "irrelevant: " << irrelevant_count << " states" << endl;
            /* Call shrink to discard unreachable and irrelevant states.
               The strategy must be one that prunes unreachable/irrelevant
               notes, but beyond that the details don't matter, as there
               is no need to actually shrink. So faster methods should be
               preferred. */

            /* TODO: Create a dedicated shrinking strategy from scratch,
               e.g. a bucket-based one that simply generates one good and
               one bad bucket? */

            // TODO/HACK: The way this is created is of course unspeakably
            // ugly. We'll leave this as is for now because there will likely
            // be more structural changes soon.
            std::shared_ptr<ShrinkStrategy> shrink_temp = ShrinkFH::create_default(num_states);
            shrink_temp->shrink(*this, num_states, true);
#ifdef DEBUG_ABSTRACTIONS
            /* We don't need to normalize here semantically, but if we don't, we won't
               get transitions_by_op[i] in sorted order which will cause warnings in the
               dump() method. To silence those warning, uncomment the following line.
               Since it changes behaviour, which is never a good thing for debug settings
               to do, it's commented out by default. */
            // normalize(false, vector<bool>());
#endif
        }
    } else {
        // skip unreachable/irrelevant pruning, but at least check for initial state being irrelevant
        if (goal_distances[init_state] >= infinity - 1) {
            vector<AbstractStateRef> abstraction_mapping(size());
            for (int i = 0; i < size(); i++) {
                abstraction_mapping[i] = i;
            }
            abstraction_mapping[init_state] = PRUNED_STATE;
            cout << tag() << "initial state irrelevant; task unsolvable" << endl;
            init_state = PRUNED_STATE;
            apply_abstraction_to_lookup_table(abstraction_mapping);
        }
    }
}

static void breadth_first_search(
    const vector<vector<int> > &graph, deque<int> &queue,
    vector<int> &distances, vector<int> &other_distances)
{
    while (!queue.empty()) {
        int state = queue.front();
        queue.pop_front();
        for (size_t i = 0; i < graph[state].size(); i++) {
            int successor = graph[state][i];
            if (!other_distances.empty() && other_distances[successor] == infinity) {
                // state already proven to be irrelevant
                continue;
            }
            if (distances[successor] > distances[state] + 1) {
                distances[successor] = distances[state] + 1;
                queue.push_back(successor);
            }
        }
    }
}

void Abstraction::compute_init_distances_unit_cost()
{
    vector<vector<AbstractStateRef> > forward_graph(num_states);
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int i = non_empty_transition->op_no;
        //for (int i = 0; i < transitions_by_op.size(); i++) {
        const vector<AbstractTransition> &transitions = transitions_by_op[i];
        for (size_t j = 0; j < transitions.size(); j++) {
            const AbstractTransition &trans = transitions[j];
            forward_graph[trans.src].push_back(trans.target);
        }
        non_empty_transition = non_empty_transition->succ;
    }

    deque<AbstractStateRef> queue;
    for (AbstractStateRef state = 0; state < num_states; state++) {
        if (state == init_state) {
            init_distances[state] = 0;
            queue.push_back(state);
        }
    }
    vector<int> other_distances;
    breadth_first_search(forward_graph, queue, init_distances, other_distances);
}

void Abstraction::compute_goal_distances_unit_cost()
{
    assert(!init_distances.empty()); // make sure that forward search is performed first
    vector<vector<AbstractStateRef> > backward_graph(num_states);
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int i = non_empty_transition->op_no;
        //for (int i = 0; i < transitions_by_op.size(); i++) {
        const vector<AbstractTransition> &transitions = transitions_by_op[i];
        for (size_t j = 0; j < transitions.size(); j++) {
            const AbstractTransition &trans = transitions[j];
            backward_graph[trans.target].push_back(trans.src);
        }
        non_empty_transition = non_empty_transition->succ;
    }

    deque<AbstractStateRef> queue;
    for (AbstractStateRef state = 0; state < num_states; state++) {
        if (goal_states[state]
                && init_distances[state] <
                infinity) { // second condition: state not proven irrelevant
            goal_distances[state] = 0;
            queue.push_back(state);
        }
    }
    breadth_first_search(backward_graph, queue, goal_distances, init_distances);
}

static void dijkstra_search(
    const vector<vector<pair<int, int> > > &graph,
    priority_queues::AdaptiveQueue<int> &queue,
    vector<int> &distances,
    vector<int> &other_distances)
{
    while (!queue.empty()) {
        pair<int, int> top_pair = queue.pop();
        int distance = top_pair.first;
        int state = top_pair.second;
        int state_distance = distances[state];
        assert(state_distance <= distance);
        if (state_distance < distance) {
            continue;
        }
        for (size_t i = 0; i < graph[state].size(); i++) {
            const pair<int, int> &transition = graph[state][i];
            int successor = transition.first;
            if (!other_distances.empty() && other_distances[successor] == infinity) {
                // state already proven to be irrelevant
                continue;
            }
            int cost = transition.second;
            int successor_cost = state_distance + cost;
            if (distances[successor] > successor_cost) {
                distances[successor] = successor_cost;
                queue.push(successor_cost, successor);
            }
        }
    }
}

void Abstraction::compute_init_distances_general_cost()
{
    vector<vector<pair<int, int> > > forward_graph(num_states);
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int i = non_empty_transition->op_no;
        //for (int i = 0; i < transitions_by_op.size(); i++) {
        int op_cost = get_adjusted_action_cost(g_operators[i], cost_type);
        const vector<AbstractTransition> &transitions = transitions_by_op[i];
        for (size_t j = 0; j < transitions.size(); j++) {
            const AbstractTransition &trans = transitions[j];
            forward_graph[trans.src].push_back(
                make_pair(trans.target, op_cost));
        }
        non_empty_transition = non_empty_transition->succ;
    }

    // TODO: Reuse the same queue for multiple computations to save speed?
    //       Also see compute_goal_distances_general_cost.
    priority_queues::AdaptiveQueue<int> queue;
    for (AbstractStateRef state = 0; state < num_states; state++) {
        if (state == init_state) {
            init_distances[state] = 0;
            queue.push(0, state);
        }
    }
    vector<int> other_distances;
    dijkstra_search(forward_graph, queue, init_distances, other_distances);
}

void Abstraction::compute_goal_distances_general_cost()
{
    assert(!init_distances.empty()); // make sure that forward search is performed first
    vector<vector<pair<int, int> > > backward_graph(num_states);
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int i = non_empty_transition->op_no;
        //for (int i = 0; i < transitions_by_op.size(); i++) {
        int op_cost = get_adjusted_action_cost(g_operators[i], cost_type);
        const vector<AbstractTransition> &transitions = transitions_by_op[i];
        for (size_t j = 0; j < transitions.size(); j++) {
            const AbstractTransition &trans = transitions[j];
            backward_graph[trans.target].push_back(
                make_pair(trans.src, op_cost));
        }
        non_empty_transition = non_empty_transition->succ;
    }

    // TODO: Reuse the same queue for multiple computations to save speed?
    //       Also see compute_init_distances_general_cost.
    priority_queues::AdaptiveQueue<int> queue;
    for (AbstractStateRef state = 0; state < num_states; state++) {
        if (goal_states[state]
                && init_distances[state] <
                infinity) { // second condition: state not proven irrelevant
            goal_distances[state] = 0;
            queue.push(0, state);
        }
    }
    dijkstra_search(backward_graph, queue, goal_distances, init_distances);
}

void find_sccs(
    vector<int> &stack,
    vector<bool> &in_stack,
    vector<std::list<AbstractStateRef> > &final_sccs,
    const vector<vector<int> > &adjacency_matrix,
    vector<int> &indices,
    vector<int> &lowlinks,
    int &index,
    const int state,
    const bool all_goal_vars_in,
    vector<bool> &is_goal,
    vector<int> &state_to_scc,
    const bool find_only_one)
{
    /*cout << "starting for state " << state << endl;*/
    indices[state] = index;
    lowlinks[state] = index;
    /*cout << "index/lowlink: " << index << endl;*/
    index++;
    stack.push_back(state);
    in_stack[state] = true;
    const vector<int> &successors = adjacency_matrix[state];
    for (size_t i = 0; i < successors.size(); i++) {
        int succ = successors[i];
        /*cout << "succ: " << succ << "; index: " << indices[succ] << endl;*/
        if (indices[succ] == -1) {
            find_sccs(stack, in_stack, final_sccs, adjacency_matrix, indices, lowlinks,
                      index, succ, all_goal_vars_in, is_goal, state_to_scc);
            if (find_only_one && !final_sccs.empty())
                // we already found one SCC and are not interested in more
            {
                return;
            }
            if (lowlinks[succ] < lowlinks[state]) {
                lowlinks[state] = lowlinks[succ];
            }
        } else {
            if (in_stack[succ] && indices[succ] < lowlinks[state]) {
                lowlinks[state] = indices[succ];
            }
            /*for (size_t s = 0; s < stack.size(); s++) {
            	int scc_state = stack[s];
            	if (scc_state == succ) {
            		if (indices[succ] < lowlinks[state])
            			lowlinks[state] = indices[succ];
            		break;
            	}
            }*/
            /*cout << "current stack:";
            for (size_t s = 0; s < scc.size(); s++) {
            	cout << " " << scc[s];
            }
            cout << endl;*/
        }
        /*cout << "lowlink of state " << state << ": " << lowlinks[state] << endl;*/
        /* update goal status */
        if (all_goal_vars_in && is_goal[succ]) {
            is_goal[state] = true;
        }
    }
    /*cout << "finishing for state " << state << endl;*/

    if (lowlinks[state] == indices[state]) {
        std::list<AbstractStateRef> new_scc;
        int num_sccs = final_sccs.size();
        while (true) {
            int popped_state = stack.back();
            assert(state_to_scc[popped_state] == -1);
            state_to_scc[popped_state] = num_sccs;
            /*cout << "popped state: " << popped_state << endl;*/
            stack.pop_back();
            in_stack[popped_state] = false;
            new_scc.push_front(popped_state);
            if (popped_state == state) {
                final_sccs.push_back(new_scc);
                break;
            }
        }
    }
    /*cout << "done for state " << state << endl;*/
}

bool Abstraction::is_scc() const
{
    vector<int> stack;
    vector<bool> in_stack(num_states, false);
    vector<std::list<AbstractStateRef> > final_sccs;
    vector<vector<int> > adjacency_matrix(num_states);
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int i = non_empty_transition->op_no;
        //for (int i = 0; i < transitions_by_op.size(); i++) {
        const vector<AbstractTransition> &transitions = transitions_by_op[i];
        for (size_t j = 0; j < transitions.size(); j++) {
            const AbstractTransition &trans = transitions[j];
            adjacency_matrix[trans.src].push_back(trans.target);
        }
        non_empty_transition = non_empty_transition->succ;
    }
    vector<int> indices(num_states, -1);
    vector<int> lowlinks(num_states, -1);
    int index = 0;
    vector<bool> is_goal(num_states, false);
    vector<int> state_to_scc(num_states, -1);
    find_sccs(stack, in_stack, final_sccs, adjacency_matrix, indices, lowlinks,
              index, init_state, all_goal_vars_in, is_goal, state_to_scc, true);
    if (final_sccs[0].size() == (size_t) num_states) { // all states in a single SCC
        return true;
    }
    return false;
}

/* This method finds SCCs and stores all successor SCCs for each of them
 * The code is based on Esko Nuutila: "An efficient transitive closure
 * algorithm for cyclic digraphs", Information Processing Letters 52 (1994),
 * 207-213 */
void Abstraction::comp_tc(
    AbstractStateRef v,
    int *current_index,
    vector<int> &index,
    vector<AbstractStateRef> &nstack,
    vector<int> &cstack,
    vector<int> &C,
    vector<int> &root,
    vector<vector<AbstractStateRef> > &successors,
    int *component_count,
    vector<set<int> > &csucc,
    EquivalenceRelation &sccs)
{
    root[v] = *current_index;
    index[v] = *current_index;
    C[v] = -1;
    nstack.push_back(v);
    int hsaved = cstack.size();
    const vector<AbstractStateRef> v_succs = successors[v];
    for (size_t i = 0; i < v_succs.size(); i++) {
        AbstractStateRef w = v_succs[i];
        bool w_unvisited = false;
        if (C[w] == -2) {
            w_unvisited = true;
            (*current_index)++;
            comp_tc(w, current_index, index, nstack, cstack, C, root, successors,
                    component_count, csucc, sccs);
        }
        if (C[w] == -1) {
            root[v] = min(root[v], root[w]);
        } else if (w_unvisited || index[w] < index[v]) { // if (v,w) not a forward edge
            cstack.push_back(C[w]);
        }
    }
    if (root[v] == index[v]) {
        set<int> csucc_set;
        //if (nstack.back() != v) {
        csucc_set.insert(*component_count);
        //}
        while (cstack.size() > (size_t) hsaved) {
            int X = cstack.back();
            cstack.pop_back();
            if (csucc_set.find(X) == csucc_set.end()) {
                csucc_set.insert(X);
                csucc_set.insert(csucc[X].begin(), csucc[X].end());
            }
        }
        csucc.push_back(csucc_set);
        AbstractStateRef w;
        EquivalenceClass scc;
        do {
            w = nstack.back(); // add to SCC
            nstack.pop_back();
            C[w] = *component_count;
            scc.push_front(w);
        } while (v != w);
        sccs.push_back(scc);
        (*component_count)++;
    }
}

bool Abstraction::apply_label_inheritance(const LabelReducer *reducer,
        bool new_approach)
{

//#define APPLY_AGGREGATION_IN_LABEL_INHERITANCE

    bool did_aggregate_states = false;

    if (new_approach) {
//#define NEW_APPROACH
//#ifdef NEW_APPROACH
        //cout << "step 1: find empty-label operators" << endl;
        vector<bool> empty_label_transition;
        empty_label_transition.resize(transitions_by_op.size(), true);
        vector<vector<AbstractStateRef> > successors(num_states);
        typedef vector<pair<AbstractStateRef, int> > GlobalStateBucket;
        vector<GlobalStateBucket> all_successors(num_states);

        if (reducer) {
            ArrayList *non_empty_transition = first_non_empty_transition;
            while (non_empty_transition) {
                int op_no = non_empty_transition->op_no;
                //for (size_t op_no = 0; op_no < transitions_by_op.size(); ++op_no) {
                empty_label_transition[op_no] = reducer->get_empty_label_operators(op_no);
                const vector<AbstractTransition> &transitions = transitions_by_op[op_no];
                for (size_t j = 0; j < transitions.size(); j++) {
                    const AbstractTransition &trans = transitions[j];
                    if (empty_label_transition[op_no]) {
                        successors[trans.src].push_back(trans.target);
                    }
#ifndef APPLY_AGGREGATION_IN_LABEL_INHERITANCE
                    all_successors[trans.src].push_back(make_pair(trans.target, op_no));
#endif
                }
                non_empty_transition = non_empty_transition->succ;
            }
        } else {
            vector<bool> is_abstraction_var;
            is_abstraction_var.resize(g_variable_name.size(), false);
            for (size_t v = 0; v < varset.size(); v++) {
                is_abstraction_var[varset[v]] = true;
            }

            /*
             * go through all operators
             * if some variable is in pre or eff that is not in abstraction it is not an empty label operator
             * store all successors of a state in additional array (for faster access later on)
             */
            ArrayList *non_empty_transition = first_non_empty_transition;
            while (non_empty_transition) {
                int op_no = non_empty_transition->op_no;
                //for (size_t op_no = 0; op_no < transitions_by_op.size(); ++op_no) {
                const GlobalOperator &op = g_operators[op_no];
                const vector<GlobalCondition> &prev = op.get_preconditions();
                for (size_t i = 0; i < prev.size(); ++i) {
                    int var = prev[i].var;
                    if (!is_abstraction_var[var]) {
                        empty_label_transition[op_no] = false;
                        break;
                    }
                }
                if (empty_label_transition[op_no]) {
                    const vector<GlobalEffect> &pre_post = op.get_effects();
                    for (size_t i = 0; i < pre_post.size(); ++i) {
                        int var = pre_post[i].var;
                        if (!is_abstraction_var[var]) {
                            empty_label_transition[op_no] = false;
                            break;
                        }
                    }
                }
                const vector<AbstractTransition> &transitions = transitions_by_op[op_no];
                for (size_t j = 0; j < transitions.size(); j++) {
                    const AbstractTransition &trans = transitions[j];
                    if (empty_label_transition[op_no]) {
                        successors[trans.src].push_back(trans.target);
                    }
#ifndef APPLY_AGGREGATION_IN_LABEL_INHERITANCE
                    all_successors[trans.src].push_back(make_pair(trans.target, op_no));
#endif
                }
                non_empty_transition = non_empty_transition->succ;
            }

            /* free some memory */
            vector<bool> ().swap(is_abstraction_var);
        }
        /* free some memory */
        vector<bool> ().swap(empty_label_transition);

        //cout << "step 2: calculate transitive closure over empty-label transitions" << endl;
        vector<AbstractStateRef> nstack;
        vector<int> cstack;
        vector<int> C;
        C.resize(num_states, -2);
        vector<int> root;
        root.resize(num_states, -1);
        int current_index = 0;
        int component_cout = 0;
        vector<set<int> > csucc;
        EquivalenceRelation sccs;
        vector<int> index;
        index.resize(num_states, -1);
        for (AbstractStateRef v = 0; v < num_states; v++) {
            if (C[v] == -2) {
                comp_tc(v, &current_index, index, nstack, cstack, C, root, successors,
                        &component_cout, csucc, sccs);
                current_index++;
            }
        }

        /* free some memory */
        vector<int> ().swap(root);
        vector<int> ().swap(index);
        vector<vector<AbstractStateRef> > ().swap(successors);

#ifdef APPLY_AGGREGATION_IN_LABEL_INHERITANCE
        /* free some memory */
        vector<int> ().swap(C);

        cout << "step 3: aggregate equal states" << endl;
        if (sccs.size() < (size_t) num_states) {
            apply_abstraction(sccs);
            did_aggregate_states = true;
        }

        /* free some memory */
        EquivalenceRelation().swap(sccs);

        cout << "step 4: handle transition relation" << endl;
        vector<set<pair<AbstractStateRef, int> > > all_successors2(num_states);
        for (size_t op_no = 0; op_no < transitions_by_op.size(); ++op_no) {
            const vector<AbstractTransition> &transitions = transitions_by_op[op_no];
            for (size_t j = 0; j < transitions.size(); j++) {
                const AbstractTransition &trans = transitions[j];
                all_successors2[trans.src].insert(make_pair(trans.target, op_no));
            }
        }
        for (int s = 0; s < num_states; s++) {
            const set<int> &csucc_set = csucc[s];
            set<int>::const_iterator csucc_it = csucc_set.begin();
            while (csucc_it != csucc_set.end()) {
                int succ = *csucc_it;
                if (succ == s) {
                    csucc_it++;
                    continue;
                }
                if (goal_states[succ]) {
                    goal_states[s] = true;
                }
                const set<pair<AbstractStateRef, int> > &all_succs = all_successors2[succ];
                set<pair<AbstractStateRef, int> >::const_iterator succ_it = all_succs.begin();
                while (succ_it != all_succs.end()) {
                    pair<AbstractStateRef, int> trans_pair = *succ_it;
                    AbstractTransition new_trans(s, trans_pair.first);
                    vector<AbstractTransition> &transitions = transitions_by_op[trans_pair.second];
                    transitions.push_back(new_trans);
                    succ_it++;
                }
                /*const GlobalStateBucket &all_succs = all_successors[succ];
                for (int j = 0; j < all_succs.size(); j++) {
                	pair<AbstractStateRef, int> trans_pair = all_succs[j];
                	AbstractTransition new_trans(s, trans_pair.first);
                	vector<AbstractTransition> &transitions = transitions_by_op[trans_pair.second];
                	transitions.push_back(new_trans);
                }*/
                csucc_it++;
            }
        }

        /* free some memory */
        vector<set<pair<AbstractStateRef, int> > > ().swap(all_successors2);
        vector<set<int> > ().swap(csucc);

#else
        //cout << "step 3: extend transition relation" << endl;
        for (AbstractStateRef s = 0; s < num_states; s++) {
            /* copy all outgoing transitions of nodes in successor SCCs (if SCC has more
             * than 1 node, this will include itself) */
            int scc_id = C[s];
            const set<int> &csucc_set = csucc[scc_id];
            set<int>::const_iterator csucc_it = csucc_set.begin();
            while (csucc_it != csucc_set.end()) {
                const EquivalenceClass &scc = sccs[*csucc_it];
                EquivalenceClass::const_iterator scc_it = scc.begin();
                for (; scc_it != scc.end(); scc_it++) {
                    AbstractStateRef succ = *scc_it;
                    if (succ == s) {
                        continue;
                    }
                    if (goal_states[succ]) {
                        goal_states[s] = true;
                    }
                    const GlobalStateBucket &all_succs = all_successors[succ];
                    for (size_t j = 0; j < all_succs.size(); j++) {
                        pair<AbstractStateRef, int> trans_pair = all_succs[j];
                        AbstractTransition new_trans(s, trans_pair.first);
                        vector<AbstractTransition> &transitions = transitions_by_op[trans_pair.second];
                        transitions.push_back(new_trans);
                    }
                }
                csucc_it++;
            }
        }

        /* free some memory */
        vector<GlobalStateBucket> ().swap(all_successors);
        vector<int> ().swap(C);
        vector<set<int> > ().swap(csucc);
        EquivalenceRelation().swap(sccs);

        /*cout << "step 3: calculate set of predecessors based on empty-label transitions" << endl;
        vector<set<AbstractStateRef> > predecessors(num_states);
        for (int i = 0; i < num_states; i++) {
        	int scc_index = C[i];
        	const vector<AbstractStateRef>& scc = sccs[scc_index];
        	for (int j = 0; j < scc.size(); j++) {
        		predecessors[scc[j]].insert(i);
        		if (goal_states[scc[j]])
        			goal_states[i] = true;
        	}
        	const set<int>& csucc_set = csucc[scc_index];
        	set<int>::const_iterator csucc_it = csucc_set.begin();
        	while (csucc_it != csucc_set.end()) {
        		const vector<AbstractStateRef>& succ_scc = sccs[*csucc_it];
        		for (int j = 0; j < succ_scc.size(); j++) {
        			predecessors[succ_scc[j]].insert(i);
        			if (goal_states[succ_scc[j]])
        				goal_states[i] = true;
        		}
        		csucc_it++;
        	}
        }*/

        /* free some memory */
        /*vector<int> ().swap(C);
        vector<set<int> > ().swap(csucc);
        vector<vector<AbstractStateRef> > ().swap(sccs);

        cout << "step 4: calculate new transition relation" << endl;
        vector<vector<AbstractTransition> > new_transitions_by_op(transitions_by_op.size());
        for (int op_no = 0; op_no < transitions_by_op.size(); op_no++) {
        	vector<AbstractTransition> &transitions = transitions_by_op[op_no];
        	vector<AbstractTransition> &new_transitions = new_transitions_by_op[op_no];
        	for (int i = 0; i < transitions.size(); i++) {
        		const AbstractTransition &trans = transitions[i];
        		const set<AbstractStateRef>& pred_set = predecessors[trans.src];
        		set<AbstractStateRef>::const_iterator pred_it = pred_set.begin();
        		while (pred_it != pred_set.end()) {
        			AbstractTransition new_trans(*pred_it, trans.target);
        			new_transitions.push_back(new_trans);
        			pred_it++;
        		}
        	}
        	vector<AbstractTransition> ().swap(transitions);
        }
        transitions_by_op.swap(new_transitions_by_op);*/
#endif
    } else {
//#else
        //cout << "step 1: find empty-label operators" << endl;
        vector<bool> empty_label_transition;
        empty_label_transition.resize(transitions_by_op.size(), true);
        typedef vector<pair<AbstractStateRef, int> > GlobalStateBucket;
        vector<GlobalStateBucket> successors(num_states);

        if (reducer) {
            ArrayList *non_empty_transition = first_non_empty_transition;
            while (non_empty_transition) {
                int op_no = non_empty_transition->op_no;
                //for (size_t op_no = 0; op_no < transitions_by_op.size(); ++op_no) {
                empty_label_transition[op_no] = reducer->get_empty_label_operators(op_no);
                const vector<AbstractTransition> &transitions = transitions_by_op[op_no];
                for (size_t j = 0; j < transitions.size(); j++) {
                    const AbstractTransition &trans = transitions[j];
                    successors[trans.src].push_back(make_pair(trans.target, op_no));
                }
                non_empty_transition = non_empty_transition->succ;
            }
        } else {
            vector<bool> is_abstraction_var;
            is_abstraction_var.resize(g_variable_name.size(), false);
            for (size_t v = 0; v < varset.size(); v++) {
                is_abstraction_var[varset[v]] = true;
            }

            /*
             * go through all operators
             * if some variable is in pre or eff that is not in abstraction it is not an empty label operator
             * store all successors of a state in additional array (for faster access later on)
             */
            ArrayList *non_empty_transition = first_non_empty_transition;
            while (non_empty_transition) {
                int op_no = non_empty_transition->op_no;
                //for (size_t op_no = 0; op_no < transitions_by_op.size(); ++op_no) {
                const GlobalOperator &op = g_operators[op_no];
                const vector<GlobalCondition> &prev = op.get_preconditions();
                for (size_t i = 0; i < prev.size(); ++i) {
                    int var = prev[i].var;
                    if (!is_abstraction_var[var]) {
                        empty_label_transition[op_no] = false;
                        break;
                    }
                }
                if (empty_label_transition[op_no]) {
                    const vector<GlobalEffect> &pre_post = op.get_effects();
                    for (size_t i = 0; i < pre_post.size(); ++i) {
                        int var = pre_post[i].var;
                        if (!is_abstraction_var[var]) {
                            empty_label_transition[op_no] = false;
                            break;
                        }
                    }
                }
                const vector<AbstractTransition> &transitions = transitions_by_op[op_no];
                for (size_t j = 0; j < transitions.size(); j++) {
                    const AbstractTransition &trans = transitions[j];
                    successors[trans.src].push_back(make_pair(trans.target, op_no));
                }
                non_empty_transition = non_empty_transition->succ;
            }

            /* free some memory */
            vector<bool> ().swap(is_abstraction_var);
        }
        //cout << "step 2: find all reachable states and (inherited) transitions for all states" << endl;
        vector<bool> reachable_states(num_states);
        deque<AbstractStateRef> states;
        for (AbstractStateRef s = 0; s < num_states; s++) {
            /*cout << "s: " << s << endl;*/
            fill(reachable_states.begin(), reachable_states.end(), false);
            reachable_states[s] = true;
            states.push_back(s);
            while (!states.empty()) {
                AbstractStateRef state = states.front();
                /*cout << "  top on stack: " << state << endl;*/
                states.pop_front();
                const vector<pair<AbstractStateRef, int> > &succs = successors[state];
                int size = succs.size();
                for (int i = 0; i < size; i++) {
                    const pair<AbstractStateRef, int> &succ_pair = succs[i];
                    AbstractStateRef statep = succ_pair.first;
                    int op_no = succ_pair.second;
                    /*cout << "    successor " << i << ": " << statep << " (by operator " << op_no << (empty_label_transition[op_no] ? "*" : "") << ")" << endl;*/
                    if (s != state) {
                        AbstractTransition trans(s, statep);
                        transitions_by_op[op_no].push_back(trans);
                        /*cout << "      added new transition " << s << " -> " << statep << endl;*/
                    }
                    if (!reachable_states[statep]) {
                        if (empty_label_transition[op_no]) {
                            /* empty label path from s to statep */
                            if (goal_states[statep]) {
                                goal_states[s] = true;
                                //break; /* no need to continue here; we do not care about outgoing transitions in goal states */
                            }
                            reachable_states[statep] = true;
                            states.push_back(statep);
                            /*cout << "      pushed on stack" << endl;*/
                        }
                    }
                }
            }
        }

        /* free some memory */
        deque<AbstractStateRef> ().swap(states);
        vector<bool> ().swap(reachable_states);
        vector<bool> ().swap(empty_label_transition);
        vector<GlobalStateBucket> ().swap(successors);
    }
//#endif

    /*bool all_goal_vars_in = true;
    for (size_t g = 0; g < g_goal.size(); g++) {
    	if (!is_in_varset(g_goal[g].first))
    		all_goal_vars_in = false;
    }*/
    if (all_goal_vars_in) {
        /*if (new_approach) {
        	cout << "step 4";
        } else {
        	cout << "step 3";
        }
        cout << ": remove outgoing transitions of goal states" << endl;*/
        vector<vector<AbstractTransition> > new_transitions_by_op(
            transitions_by_op.size());
        ArrayList *non_empty_transition = first_non_empty_transition;
        while (non_empty_transition) {
            int op_no = non_empty_transition->op_no;
            //for (int op_no = 0; op_no < transitions_by_op.size(); op_no++) {
            vector<AbstractTransition> &transitions = transitions_by_op[op_no];
            vector<AbstractTransition> &new_transitions = new_transitions_by_op[op_no];
            for (size_t i = 0; i < transitions.size(); i++) {
                const AbstractTransition &trans = transitions[i];
                if (goal_states[trans.src]) {
                    continue;
                }
                AbstractTransition new_trans(trans.src, trans.target);
                new_transitions.push_back(new_trans);
            }
            vector<AbstractTransition> ().swap(transitions);
            non_empty_transition = non_empty_transition->succ;
        }
        transitions_by_op.swap(new_transitions_by_op);
    }

    // label inheritance is not f-preserving; must recalculate distances later on
    clear_distances();

    return did_aggregate_states;
}

void AtomicAbstraction::apply_abstraction_to_lookup_table(const vector <
        AbstractStateRef > &abstraction_mapping)
{
    cout << tag() << "applying abstraction to lookup table" << endl;
    for (size_t i = 0; i < lookup_table.size(); i++) {
        AbstractStateRef old_state = lookup_table[i];
        if (old_state != PRUNED_STATE) {
            lookup_table[i] = abstraction_mapping[old_state];
        }
    }
}

void CompositeAbstraction::apply_abstraction_to_lookup_table(
    const vector<AbstractStateRef> &abstraction_mapping)
{
    cout << tag() << "applying abstraction to lookup table" << endl;
    for (int i = 0; i < components[0]->size(); i++) {
        for (int j = 0; j < components[1]->size(); j++) {
            AbstractStateRef old_state = lookup_table[i][j];
            if (old_state != PRUNED_STATE) {
                lookup_table[i][j] = abstraction_mapping[old_state];
            }
        }
    }
}

//void Abstraction::remove_goal_outgoing_transitions() {
//    typedef vector<pair<AbstractStateRef, int> > GlobalStateBucket;
//
//    /* First, partition by target state. Also replace operators by
//       their canonical representatives via label reduction and clear
//       away the transitions that have been processed. */
//    vector<GlobalStateBucket> target_buckets(num_states);
//    for (int op_no = 0; op_no < transitions_by_op.size(); op_no++) {
//        vector<AbstractTransition> &transitions = transitions_by_op[op_no];
//        if (!transitions.empty()) {
//            for (int i = 0; i < transitions.size(); i++) {
//                const AbstractTransition &t = transitions[i];
//            	if (goal_states[t.src]) // we want to remove the outgoing actions of all goal states
//            		continue;
//                target_buckets[t.target].push_back(
//                    make_pair(t.src, op_no));
//            }
//            vector<AbstractTransition> ().swap(transitions);
//        }
//    }
//
//    // Second, partition by src state.
//    vector<GlobalStateBucket> src_buckets(num_states);
//
//    for (AbstractStateRef target = 0; target < num_states; target++) {
//        GlobalStateBucket &bucket = target_buckets[target];
//        for (int i = 0; i < bucket.size(); i++) {
//            AbstractStateRef src = bucket[i].first;
//            int op_no = bucket[i].second;
//            src_buckets[src].push_back(make_pair(target, op_no));
//        }
//    }
//    vector<GlobalStateBucket> ().swap(target_buckets);
//
//    // Finally, partition by operator and drop duplicates.
//    for (AbstractStateRef src = 0; src < num_states; src++) {
//        GlobalStateBucket &bucket = src_buckets[src];
//        for (int i = 0; i < bucket.size(); i++) {
//            int target = bucket[i].first;
//            int op_no = bucket[i].second;
//
//            vector<AbstractTransition> &op_bucket = transitions_by_op[op_no];
//            AbstractTransition trans(src, target);
//            if (op_bucket.empty() || op_bucket.back() != trans)
//                op_bucket.push_back(trans);
//        }
//    }
//}

void Abstraction::normalize(bool reduce_labels,
                            const std::vector<bool> &caught_labels,
                            bool use_label_inheritance, bool new_approach)
{
    // Apply label reduction and remove duplicate transitions.

#ifdef DEBUG_ABSTRACTIONS
    // Commented out since we shouldn't usually need/want this, even when debugging.
    // It will also give us warnings since the abstraction is not normalized here.
    /*
    cout << "DUMPING BEFORE NORMALIZATION" << endl;
    dump();
    */
#endif

    //cout << "before normalization" << endl;
    // Peter: number of states and/or transitions cannot have increased, so no need to update statistics here
    //statistics(false);

    LabelReducer *reducer = 0;

    cout << tag() << "normalizing ";
    if (reduce_labels) {
        if (are_labels_reduced) {
            cout << "without label reduction (already reduced)" << endl;
        } else {
            cout << "with label reduction" << endl;
            reducer = new LabelReducer(relevant_operators, varset, cost_type, caught_labels,
                                       vars_label_operators);
        }
    } else {
        cout << "without label reduction" << endl;
    }

    if (reducer) {
        reducer->statistics();
        are_labels_reduced = true;
    }

    typedef vector<pair<AbstractStateRef, int> > GlobalStateBucket;

    /* First, partition by target state. Also replace operators by
       their canonical representatives via label reduction and clear
       away the transitions that have been processed. */
    vector<GlobalStateBucket> target_buckets(num_states);
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int op_no = non_empty_transition->op_no;
        // updating to successor already, as it might be deleted later on
        non_empty_transition = non_empty_transition->succ;
        //for (int op_no = 0; op_no < transitions_by_op.size(); op_no++) {
        vector<AbstractTransition> &transitions = transitions_by_op[op_no];
        //if (!transitions.empty()) {
        int reduced_op_no;
        if (reducer) {
            const GlobalOperator *op = &g_operators[op_no];
            const GlobalOperator *reduced_op = reducer->get_reduced_label(op);
            reduced_op_no = get_op_index(reduced_op);
        } else {
            reduced_op_no = op_no;
        }
        if (reduced_op_no != op_no) {
            // remove element from list of non-empty transitions
            // must be the same as non_empty_transition at the start of the loop
            ArrayList *now_empty_transition = non_empty_transitions[op_no];
            if (now_empty_transition->prev) {
                now_empty_transition->prev->succ = now_empty_transition->succ;
            } else {
                first_non_empty_transition = now_empty_transition->succ;
            }
            if (now_empty_transition->succ) {
                now_empty_transition->succ->prev = now_empty_transition->prev;
            }
            //non_empty_transition = now_empty_transition->prev;
            delete now_empty_transition;
            non_empty_transitions[op_no] = NULL;

            if (!non_empty_transitions[reduced_op_no]) {
                // previously empty transition is no longer empty now
                ArrayList *new_non_empty_transition = new ArrayList();
                new_non_empty_transition->op_no = reduced_op_no;
                int prev_pos = -1;
                for (int i = reduced_op_no - 1; i >= 0; i--) {
                    if (non_empty_transitions[i]) {
                        prev_pos = i;
                        break;
                    }
                }
                if (prev_pos == -1) {
                    new_non_empty_transition->prev = NULL;
                    new_non_empty_transition->succ = first_non_empty_transition;
                    first_non_empty_transition = new_non_empty_transition;
                } else {
                    new_non_empty_transition->prev = non_empty_transitions[prev_pos];
                    new_non_empty_transition->succ = non_empty_transitions[prev_pos]->succ;
                    non_empty_transitions[prev_pos]->succ = new_non_empty_transition;
                }
                if (new_non_empty_transition->succ) {
                    new_non_empty_transition->succ->prev = new_non_empty_transition;
                }
                non_empty_transitions[reduced_op_no] = new_non_empty_transition;
            }
        }
        for (size_t i = 0; i < transitions.size(); i++) {
            const AbstractTransition &t = transitions[i];
            /*if (all_goal_vars_in && goal_states[t.src]) {
            	// goal state will always remain goal state, which we cannot leave
            	continue;
            }*/
            target_buckets[t.target].push_back(
                make_pair(t.src, reduced_op_no));
        }
        vector<AbstractTransition> ().swap(transitions);
        //}
        //non_empty_transition = non_empty_transition->succ;
    }

    // Second, partition by src state.
    vector<GlobalStateBucket> src_buckets(num_states);

    for (AbstractStateRef target = 0; target < num_states; target++) {
        GlobalStateBucket &bucket = target_buckets[target];
        for (size_t i = 0; i < bucket.size(); i++) {
            AbstractStateRef src = bucket[i].first;
            int op_no = bucket[i].second;
            src_buckets[src].push_back(make_pair(target, op_no));
        }
    }
    vector<GlobalStateBucket> ().swap(target_buckets);

    // Finally, partition by operator and drop duplicates.
    for (AbstractStateRef src = 0; src < num_states; src++) {
        GlobalStateBucket &bucket = src_buckets[src];
        for (size_t i = 0; i < bucket.size(); i++) {
            int target = bucket[i].first;
            int op_no = bucket[i].second;

            vector<AbstractTransition> &op_bucket = transitions_by_op[op_no];
            AbstractTransition trans(src, target);
            if (op_bucket.empty() || op_bucket.back() != trans) {
                op_bucket.push_back(trans);
            }
        }
    }

    if (use_label_inheritance) {
        // Peter: statistics has as side-effect that they calculate the maximum number of states and
        // transitions. For that need to call statistics only whenever these sizes can increase.
        // That can only happen in case of label inheritance, giving us new transitions; in all
        // other cases the sizes can only decrease, and we do not care about this.
        //statistics(false);
        /*bool did_aggregate_states =*/ apply_label_inheritance(reducer, new_approach);
        /*if (reducer && did_aggregate_states) { // shrinking cannot change the output of the label reducer!
        	delete reducer;
        	reducer = new LabelReducer(relevant_operators, varset, cost_type, caught_labels);
        }*/
        //cout << "after applying label inheritance" << endl;
        statistics(false);
        //cout << "need to normalize again" << endl;
        normalize(reduce_labels, caught_labels, false, new_approach);
    }

    delete reducer;
#ifdef DEBUG_ABSTRACTIONS
    cout << "DUMPING AFTER NORMALIZATION" << endl;
    dump();
#endif
}

void Abstraction::build_atomic_abstractions(
    bool is_unit_cost, OperatorCost cost_type,
    vector<Abstraction *> &result)
{
    assert(result.empty());
    cout << "Building atomic abstractions... " << endl;
    int var_count = g_variable_domain.size();

    // Step 1: Create the abstraction objects without transitions.
    for (int var_no = 0; var_no < var_count; var_no++) {
        result.push_back(new AtomicAbstraction(
                             is_unit_cost, cost_type, var_no));
    }

    // Step 2: Add transitions.
    std::vector<int> precondition(g_variable_domain.size(), -1);
    std::vector<int> effect(g_variable_domain.size(), -1);
    std::set<int> relevant_variables;
    for (size_t op_no = 0; op_no < g_operators.size(); op_no++) {
        relevant_variables.clear();
        std::fill(precondition.begin(), precondition.end(), -1);
        std::fill(effect.begin(), effect.end(), -1);
        const GlobalOperator *op = &g_operators[op_no];
        for (const auto& pre : op->get_preconditions()) {
            precondition[pre.var] = pre.val;
            effect[pre.var] = pre.val;
            relevant_variables.insert(pre.var);
        }
        for (const auto& eff : op->get_effects()) {
            effect[eff.var] = eff.val;
            relevant_variables.insert(eff.var);
        }
        for (int var : relevant_variables) {
            if (precondition[var] == effect[var]) {
                Abstraction *abs = result[var];
                /*if (abs->get_all_goal_vars_in() && abs->is_goal_state(value)) {
                    // goal state will always remain goal state, which we cannot leave
                    continue;
                }*/
                AbstractTransition trans(precondition[var], precondition[var]);
                abs->transitions_by_op[op_no].push_back(trans);
                if (abs->relevant_operators.empty()
                        || abs->relevant_operators.back() != op) {
                    abs->relevant_operators.push_back(op);
                }
            } else {
                int pre_value = precondition[var];
                int post_value = effect[var];
                Abstraction *abs = result[var];
                int pre_value_min, pre_value_max;
                if (pre_value == -1) {
                    pre_value_min = 0;
                    pre_value_max = g_variable_domain[var];
                } else {
                    pre_value_min = pre_value;
                    pre_value_max = pre_value + 1;
                }
                bool added_transition = false;
                for (int value = pre_value_min; value < pre_value_max; value++) {
                    /*if (abs->get_all_goal_vars_in() && abs->is_goal_state(value)) {
                        // goal state will always remain goal state, which we cannot leave
                        continue;
                    }*/
                    AbstractTransition trans(value, post_value);
                    abs->transitions_by_op[op_no].push_back(trans);
                    added_transition = true;
                }
                if (added_transition && (abs->relevant_operators.empty()
                                        || abs->relevant_operators.back() != op)) {
                    abs->relevant_operators.push_back(op);
                }
            }
        }
    }
    for (size_t i = 0; i < result.size(); i++) {
        Abstraction *abs = result[i];
        // Peter: store which transitions are not empty for faster access in normalize and statistics
        abs->non_empty_transitions.resize(g_operators.size());
        abs->first_non_empty_transition = NULL;
        ArrayList *last_non_empty = NULL;
        for (size_t op_no = 0; op_no < g_operators.size(); op_no++) {
            if (abs->transitions_by_op[op_no].empty()) {
                // this element does not exist -> set it to NULL
                abs->non_empty_transitions[op_no] = NULL;
            } else {
                // this element does exist; set its predecessor to the last non-empty element, its successor to the last element,
                // its predecessor's successor to this, and the last element's predecessor to this
                abs->non_empty_transitions[op_no] = new ArrayList();
                abs->non_empty_transitions[op_no]->op_no = op_no;
                abs->non_empty_transitions[op_no]->prev = last_non_empty;
                abs->non_empty_transitions[op_no]->succ = NULL;
                if (last_non_empty) {
                    last_non_empty->succ = abs->non_empty_transitions[op_no];
                } else {
                    abs->first_non_empty_transition = abs->non_empty_transitions[op_no];
                }
                last_non_empty = abs->non_empty_transitions[op_no];
            }
        }
    }
}

AtomicAbstraction::AtomicAbstraction(
    bool is_unit_cost, OperatorCost cost_type, int variable_)
    : Abstraction(is_unit_cost, cost_type), variable(variable_)
{
    if (g_goal.size() == 1 && g_goal[0].first == variable) {
        all_goal_vars_in = true;
    } else {
        all_goal_vars_in = false;
    }
    varset.push_back(variable);
    /*
      This generates the states of the atomic abstraction, but not the
      arcs: It is more efficient to generate all arcs of all atomic
      abstractions simultaneously.
     */
    int range = g_variable_domain[variable];

    int init_value = (g_initial_state())[variable];
    int goal_value = -1;
    for (size_t goal_no = 0; goal_no < g_goal.size(); goal_no++) {
        if (g_goal[goal_no].first == variable) {
            assert(goal_value == -1);
            goal_value = g_goal[goal_no].second;
        }
    }

    num_states = range;
    lookup_table.reserve(range);
    goal_states.resize(num_states, false);
    for (int value = 0; value < range; value++) {
        if (value == goal_value || goal_value == -1) {
            goal_states[value] = true;
        }
        if (value == init_value) {
            init_state = value;
        }
        lookup_table.push_back(value);
    }
}

AtomicAbstraction::~AtomicAbstraction()
{
}

CompositeAbstraction::CompositeAbstraction(
    bool is_unit_cost, OperatorCost cost_type,
    Abstraction *abs1, Abstraction *abs2)
    : Abstraction(is_unit_cost, cost_type)
{
    cout << "Merging " << abs1->description() << " and "
         << abs2->description() << endl;

    assert(abs1->is_solvable() && abs2->is_solvable());

    components[0] = abs1;
    components[1] = abs2;

    ::set_union(abs1->varset.begin(), abs1->varset.end(), abs2->varset.begin(),
                abs2->varset.end(), back_inserter(varset));
    //bool newly_all_goal_vars_in = false;
    if (abs1->get_all_goal_vars_in() || abs2->get_all_goal_vars_in()) {
        all_goal_vars_in = true;
        //newly_all_goal_vars_in = false;
    } else {
        all_goal_vars_in = true;
        for (size_t g = 0; g < g_goal.size(); g++) {
            if (!is_in_varset(g_goal[g].first)) {
                all_goal_vars_in = false;
                break;
            }
        }
        /*if (all_goal_vars_in)
        	newly_all_goal_vars_in = true;*/
    }

    num_states = abs1->size() * abs2->size();
    goal_states.resize(num_states, false);

    lookup_table.resize(abs1->size(), vector<AbstractStateRef> (abs2->size()));
    init_state =
        PRUNED_STATE; // default value -- in case it is pruned in one of the abstractions
    for (int s1 = 0; s1 < abs1->size(); s1++) {
        for (int s2 = 0; s2 < abs2->size(); s2++) {
            int state = s1 * abs2->size() + s2;
            lookup_table[s1][s2] = state;
            if (abs1->goal_states[s1] && abs2->goal_states[s2]) {
                goal_states[state] = true;
            }
            if (s1 == abs1->init_state && s2 == abs2->init_state) {
                init_state = state;
            }
        }
    }

    std::vector<bool> marker1(g_operators.size(), false);
    std::vector<bool> marker2(g_operators.size(), false);
    for (size_t i = 0; i < abs1->relevant_operators.size(); i++) {
        marker1[get_op_index(abs1->relevant_operators[i])] = true;
    }
    for (size_t i = 0; i < abs2->relevant_operators.size(); i++) {
        marker2[get_op_index(abs2->relevant_operators[i])] = true;
    }

    // Peter: store which transitions are not empty for faster access in normalize and statistics
    non_empty_transitions.resize(g_operators.size());
    first_non_empty_transition = NULL;
    ArrayList *last_non_empty = NULL;
    int multiplier = abs2->size();
    for (size_t op_no = 0; op_no < g_operators.size(); op_no++) {
        const GlobalOperator *op = &g_operators[op_no];
        bool relevant1 = marker1[op_no];
        bool relevant2 = marker2[op_no];
        if (relevant1 || relevant2) {
            // label reduction of abs2 not necessary, as all actions being reduced to the
            // same action in abs1 must be equal in abs2
            vector<AbstractTransition> &transitions = transitions_by_op[op_no];
            relevant_operators.push_back(op);
            const vector<AbstractTransition> &bucket1 =
                abs1->transitions_by_op[op_no];
            const vector<AbstractTransition> &bucket2 =
                abs2->transitions_by_op[op_no];
            if (relevant1 && relevant2) {
                transitions.reserve(bucket1.size() * bucket2.size());
                for (size_t i = 0; i < bucket1.size(); i++) {
                    int src1 = bucket1[i].src;
                    int target1 = bucket1[i].target;
                    for (size_t j = 0; j < bucket2.size(); j++) {
                        int src2 = bucket2[j].src;
                        int target2 = bucket2[j].target;
                        int src = src1 * multiplier + src2;
                        /*if (newly_all_goal_vars_in && goal_states[src]) {
                        	// goal state will always remain goal state, which we cannot leave
                        	continue;
                        }*/
                        int target = target1 * multiplier + target2;
                        transitions.push_back(AbstractTransition(src, target));
                    }
                }
            } else if (relevant1) {
                assert(!relevant2);
                transitions.reserve(bucket1.size() * abs2->size());
                for (size_t i = 0; i < bucket1.size(); i++) {
                    int src1 = bucket1[i].src;
                    int target1 = bucket1[i].target;
                    for (int s2 = 0; s2 < abs2->size(); s2++) {
                        int src = src1 * multiplier + s2;
                        /*if (newly_all_goal_vars_in && goal_states[src]) {
                        	// goal state will always remain goal state, which we cannot leave
                        	continue;
                        }*/
                        int target = target1 * multiplier + s2;
                        transitions.push_back(AbstractTransition(src, target));
                    }
                }
            } else if (relevant2) {
                assert(!relevant1);
                transitions.reserve(bucket2.size() * abs1->size());
                for (size_t i = 0; i < bucket2.size(); i++) {
                    int src2 = bucket2[i].src;
                    int target2 = bucket2[i].target;
                    for (int s1 = 0; s1 < abs1->size(); s1++) {
                        int src = s1 * multiplier + src2;
                        /*if (newly_all_goal_vars_in && goal_states[src]) {
                        	// goal state will always remain goal state, which we cannot leave
                        	continue;
                        }*/
                        int target = s1 * multiplier + target2;
                        transitions.push_back(AbstractTransition(src, target));
                    }
                }
            }
        }
        if (transitions_by_op[op_no].empty()) {
            // this element does not exist -> set it to NULL
            non_empty_transitions[op_no] = NULL;
        } else {
            // this element does exist; set its predecessor to the last non-empty element, its successor to the last element,
            // its predecessor's successor to this, and the last element's predecessor to this
            non_empty_transitions[op_no] = new ArrayList();
            non_empty_transitions[op_no]->op_no = op_no;
            non_empty_transitions[op_no]->prev = last_non_empty;
            non_empty_transitions[op_no]->succ = NULL;
            if (last_non_empty) {
                last_non_empty->succ = non_empty_transitions[op_no];
            } else {
                first_non_empty_transition = non_empty_transitions[op_no];
            }
            last_non_empty = non_empty_transitions[op_no];
        }
    }
}

CompositeAbstraction::~CompositeAbstraction()
{
}

string AtomicAbstraction::description() const
{
    ostringstream s;
    s << "atomic abstraction #" << variable;
    return s.str();
}

string CompositeAbstraction::description() const
{
    ostringstream s;
    s << "abstraction (" << varset.size() << "/"
      << g_variable_domain.size() << " vars)";
    return s.str();
}

AbstractStateRef AtomicAbstraction::get_abstract_state(const GlobalState &state)
const
{
    int value = state[variable];
    return lookup_table[value];
}

AbstractStateRef CompositeAbstraction::get_abstract_state(
    const GlobalState &state) const
{
    AbstractStateRef state1 = components[0]->get_abstract_state(state);
    AbstractStateRef state2 = components[1]->get_abstract_state(state);
    if (state1 == PRUNED_STATE || state2 == PRUNED_STATE) {
        return PRUNED_STATE;
    }
    return lookup_table[state1][state2];
}

void Abstraction::apply_abstraction(
    vector<std::list<AbstractStateRef> > &collapsed_groups)
{
    /* Note on how this method interacts with the distance information
       (init_distances and goal_distances): if no two states with
       different g or h values are combined by the abstraction (i.e.,
       if the abstraction is "f-preserving", then this method makes
       sure sure that distance information is preserved.

       This is important because one of the (indirect) callers of this
       method is the distance computation code, which uses it in a
       somewhat roundabout way to get rid of irrelevant and
       unreachable states. That caller will always give us an
       f-preserving abstraction.

       When called with a non-f-preserving abstraction, distance
       information is cleared as a side effect. In most cases we won't
       actually need it any more at this point anyway, so it is no
       great loss.

       Still, it might be good if we could find a way to perform the
       unreachability and relevance pruning that didn't introduce such
       tight coupling between the distance computation and abstraction
       code. It would probably also a good idea to do the
       unreachability/relevance pruning as early as possible, e.g.
       right after construction.
     */

    cout << tag() << "applying abstraction (" << size()
         << " to " << collapsed_groups.size() << " states)" << endl;

    typedef std::list<AbstractStateRef> Group;

    vector<int> abstraction_mapping(num_states, PRUNED_STATE);
    for (size_t group_no = 0; group_no < collapsed_groups.size(); group_no++) {
        Group &group = collapsed_groups[group_no];
        for (Group::iterator pos = group.begin(); pos != group.end(); ++pos) {
            AbstractStateRef state = *pos;
            assert(abstraction_mapping[state] == PRUNED_STATE);
            abstraction_mapping[state] = group_no;
        }
    }

    int new_num_states = collapsed_groups.size();
    vector<bool> new_goal_states(new_num_states, false);

    bool must_clear_distances = false;
    bool was_h_preserving = true;

    if (init_distances.empty()) { // distances already cleared; cannot copy any distance infos
        for (size_t new_state = 0; new_state < collapsed_groups.size();
                new_state++) {
            Group &group = collapsed_groups[new_state];
            assert(!group.empty());

            Group::iterator pos = group.begin();
            new_goal_states[new_state] = goal_states[*pos];

            ++pos;
            for (; pos != group.end(); ++pos) {
                if (goal_states[*pos]) {
                    new_goal_states[new_state] = true;
                }
            }
        }
    } else {
        vector<int> new_init_distances(new_num_states, infinity);
        vector<int> new_goal_distances(new_num_states, infinity);
        for (size_t new_state = 0; new_state < collapsed_groups.size();
                new_state++) {
            Group &group = collapsed_groups[new_state];
            assert(!group.empty());

            Group::iterator pos = group.begin();
            int &new_init_dist = new_init_distances[new_state];
            int &new_goal_dist = new_goal_distances[new_state];

            new_init_dist = init_distances[*pos];
            new_goal_dist = goal_distances[*pos];
            new_goal_states[new_state] = goal_states[*pos];

            ++pos;
            for (; pos != group.end(); ++pos) {
                if (init_distances[*pos] < new_init_dist) {
                    must_clear_distances = true;
                    new_init_dist = init_distances[*pos];
                }
                if (goal_distances[*pos] < new_goal_dist) {
                    must_clear_distances = true;
                    was_h_preserving = false;
                    new_goal_dist = goal_distances[*pos];
                }
                if (goal_states[*pos]) {
                    new_goal_states[new_state] = true;
                }
            }
        }
        // Release memory.
        vector<int>().swap(init_distances);
        vector<int>().swap(goal_distances);

        init_distances.swap(new_init_distances);
        goal_distances.swap(new_goal_distances);
    }

    // Release memory.
    vector<bool>().swap(goal_states);

    goal_states.swap(new_goal_states);

    vector<vector<AbstractTransition> > new_transitions_by_op(
        transitions_by_op.size());
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int op_no = non_empty_transition->op_no;
        //for (int op_no = 0; op_no < transitions_by_op.size(); op_no++) {
        const vector<AbstractTransition> &transitions =
            transitions_by_op[op_no];
        vector<AbstractTransition> &new_transitions =
            new_transitions_by_op[op_no];
        new_transitions.reserve(transitions.size());
        for (size_t i = 0; i < transitions.size(); i++) {
            const AbstractTransition &trans = transitions[i];
            int src = abstraction_mapping[trans.src];
            int target = abstraction_mapping[trans.target];
            if (src != PRUNED_STATE && target != PRUNED_STATE) {
                /*if (all_goal_vars_in && goal_states[src]) {
                	// goal state will always remain goal state, which we cannot leave
                	continue;
                }*/
                new_transitions.push_back(AbstractTransition(src, target));
            }
        }
        if (new_transitions.empty()) {
            // all src and target states pruned
            ArrayList *tmp_pointer = non_empty_transition;
            non_empty_transition = non_empty_transition->succ;
            if (tmp_pointer->prev) {
                tmp_pointer->prev->succ = tmp_pointer->succ;
            } else {
                first_non_empty_transition = tmp_pointer->succ;
            }
            if (tmp_pointer->succ) {
                tmp_pointer->succ->prev = tmp_pointer->prev;
            }
            delete tmp_pointer;
            non_empty_transitions[op_no] = NULL;
        } else {
            non_empty_transition = non_empty_transition->succ;
        }
    }
    vector<vector<AbstractTransition> > ().swap(transitions_by_op);
    transitions_by_op.swap(new_transitions_by_op);

    num_states = new_num_states;
    if (init_state != PRUNED_STATE) {
        init_state = abstraction_mapping[init_state];
        if (init_state == PRUNED_STATE) {
            cout << tag() << "initial state pruned; task unsolvable" << endl;
        }
    }

    apply_abstraction_to_lookup_table(abstraction_mapping);

    if (must_clear_distances) {
        cout << tag() << "simplification was not f-preserving!" << endl;
        if (!was_h_preserving) {
            cout << tag() << "simplification was not h-preserving!" << endl;
        }
        clear_distances();
    }
}

bool Abstraction::is_solvable() const
{
    return init_state != PRUNED_STATE;
}

int Abstraction::get_cost(const GlobalState &state) const
{
    int abs_state = get_abstract_state(state);
    if (abs_state == PRUNED_STATE) {
        return -1;
    }
    int cost = goal_distances[abs_state];
    assert(cost != infinity);
    return cost;
}

int Abstraction::memory_estimate() const
{
    int result = sizeof(Abstraction);
    result += sizeof(GlobalOperator *) * relevant_operators.capacity();
    result += sizeof(vector<AbstractTransition>)
              * transitions_by_op.capacity();
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int i = non_empty_transition->op_no;
        //for (int i = 0; i < transitions_by_op.size(); i++)
        result += sizeof(AbstractTransition) * transitions_by_op[i].capacity();
        non_empty_transition = non_empty_transition->succ;
    }
    result += sizeof(int) * init_distances.capacity();
    result += sizeof(int) * goal_distances.capacity();
    result += sizeof(bool) * goal_states.capacity();
    return result;
}

int AtomicAbstraction::memory_estimate() const
{
    int result = Abstraction::memory_estimate();
    result += sizeof(AtomicAbstraction) - sizeof(Abstraction);
    result += sizeof(AbstractStateRef) * lookup_table.capacity();
    return result;
}

int CompositeAbstraction::memory_estimate() const
{
    int result = Abstraction::memory_estimate();
    result += sizeof(CompositeAbstraction) - sizeof(Abstraction);
    result += sizeof(vector<AbstractStateRef>) * lookup_table.capacity();
    for (size_t i = 0; i < lookup_table.size(); i++) {
        result += sizeof(AbstractStateRef) * lookup_table[i].capacity();
    }
    return result;
}

void Abstraction::release_memory()
{
    vector<const GlobalOperator *>().swap(relevant_operators);
    vector<vector<AbstractTransition> >().swap(transitions_by_op);
}

int Abstraction::total_transitions() const
{
    int total = 0;
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int op_no = non_empty_transition->op_no;
        total += transitions_by_op[op_no].size();
        non_empty_transition = non_empty_transition->succ;
    }
    /*for (int i = 0; i < transitions_by_op.size(); i++)
        total += transitions_by_op[i].size();*/
    return total;
}

int Abstraction::unique_unlabeled_transitions() const
{
    vector<AbstractTransition> unique_transitions;
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int i = non_empty_transition->op_no;
        //for (int i = 0; i < transitions_by_op.size(); i++) {
        const vector<AbstractTransition> &trans = transitions_by_op[i];
        unique_transitions.insert(unique_transitions.end(), trans.begin(),
                                  trans.end());
        non_empty_transition = non_empty_transition->succ;
    }
    ::sort(unique_transitions.begin(), unique_transitions.end());
    return unique(unique_transitions.begin(), unique_transitions.end())
           - unique_transitions.begin();
}

void Abstraction::statistics(bool include_expensive_statistics) const
{
    int memory = memory_estimate();
    peak_memory = max(peak_memory, memory);
    cout << tag() << size() << " states, ";
    max_num_states = max(max_num_states, size());
    if (include_expensive_statistics) {
        cout << unique_unlabeled_transitions();
    } else {
        cout << "???";
    }
    int num_transitions = total_transitions();
    max_num_transitions = max(max_num_transitions, num_transitions);
    cout << "/" << num_transitions << " arcs, " << memory << " bytes"
         << endl;
    cout << tag();
    if (max_h == DISTANCE_UNKNOWN) {
        cout << "distances not computed";
    } else if (is_solvable()) {
        cout << "init h=" << goal_distances[init_state] << ", max f=" << max_f
             << ", max g=" << max_g << ", max h=" << max_h;
    } else {
        cout << "abstraction is unsolvable";
    }
    cout << " [t=" << utils::g_timer << "]" << endl;
}

int Abstraction::get_peak_memory_estimate() const
{
    return peak_memory;
}

int Abstraction::get_max_num_states() const
{
    return max_num_states;
}

int Abstraction::get_max_num_transitions() const
{
    return max_num_transitions;
}

int Abstraction::get_overall_peak_memory_estimate() const
{
    return max(peak_memory, overall_peak_memory);
}

int Abstraction::get_overall_max_num_states() const
{
    return max(max_num_states, overall_max_num_states);
}

int Abstraction::get_overall_max_num_transitions() const
{
    return max(max_num_transitions, overall_max_num_transitions);
}

void Abstraction::set_overall_peak_memory_estimate(int peak_memory)
{
    this->overall_peak_memory = peak_memory;
}

void Abstraction::set_overall_max_num_states(int max_num_states)
{
    this->overall_max_num_states = max_num_states;
}

void Abstraction::set_overall_max_num_transitions(int max_num_transitions)
{
    this->overall_max_num_transitions = max_num_transitions;
}

bool Abstraction::is_in_varset(int var) const
{
    return find(varset.begin(), varset.end(), var) != varset.end();
}

void Abstraction::dump() const
{
    cout << "digraph abstract_transition_graph";
    for (size_t i = 0; i < varset.size(); i++) {
        cout << "_" << varset[i];
    }
    cout << " {" << endl;
    cout << "    node [shape = none] start;" << endl;
    for (int i = 0; i < num_states; i++) {
        bool is_init = (i == init_state);
        bool is_goal = (goal_states[i] == true);
        cout << "    node [shape = " << (is_goal ? "doublecircle" : "circle")
             << "] node" << i << ";   // ";
        if (goal_distances.empty()) {
            cout << " no goal distances computed";
        } else {
            cout << get_goal_distance(i);
        }
        cout << endl;
        if (is_init) {
            cout << "    start -> node" << i << ";" << endl;
        }
    }
    assert(transitions_by_op.size() == g_operators.size());
    for (size_t op_no = 0; op_no < g_operators.size(); op_no++) {
        const vector<AbstractTransition> &trans = transitions_by_op[op_no];
        for (size_t i = 0; i < trans.size(); i++) {
            int src = trans[i].src;
            int target = trans[i].target;
            cout << "    node" << src << " -> node" << target << " [label =\"o_"
                 << op_no <<
                 "(" << get_adjusted_action_cost(g_operators[op_no], cost_type) << ")\"" <<
                 "];" << endl;

            if (i > 0) {
                if (src < trans[i - 1].src ||
                        (src == trans[i - 1].src && target < trans[i - 1].target)) {
                    cout << "// *** warning: not sorted ***" << endl;
                } else if (src == trans[i - 1].src &&
                           target == trans[i - 1].target) {
                    cout << "// *** warning: duplicate ***" << endl;
                }
            }
        }
    }
    cout << "}" << endl;
}


/* Additional dump method added by Michael.
   TODO: If we're going to keep this, it must be consolidated with the above dump() method.
*/

void Abstraction::dump(ostream &ou) const
{
    ou << "digraph abstract_transition_graph";
    for (size_t i = 0; i < varset.size(); i++) {
        ou << "_" << varset[i];
    }
    ou << " {" << endl;
    ou << "    node [shape = none] start;" << endl;
    vector<vector<vector<int> > > ops_by_src_target;

    ops_by_src_target.resize(num_states);
    for (int i = 0; i < num_states; i++) {
        ops_by_src_target[i].resize(num_states);
        bool is_init = (i == init_state);
        bool is_goal = (goal_states[i] == true);
        ou << "    node" << i << " [shape = " << (is_goal ? "doublecircle" : "circle")
           << ",label=\"" << i << "(" << get_goal_distance(i) << ")\"];" << endl;
        if (is_init) {
            ou << "    start -> node" << i << ";" << endl;
        }
    }
    assert(transitions_by_op.size() == g_operators.size());

    for (size_t op_no = 0; op_no < g_operators.size(); op_no++) {
        const vector<AbstractTransition> &trans = transitions_by_op[op_no];
        for (size_t i = 0; i < trans.size(); i++) {
            int src = trans[i].src;
            int target = trans[i].target;
            ops_by_src_target[src][target].push_back(op_no);
        }
    }
    for (int src = 0; src < num_states; src++) {
        for (int target = 0; target < num_states; target++) {
            if (ops_by_src_target[src][target].size() == 0) {
                continue;
            }

            ou << "    node" << src << " -> node" << target << " [label =\"";
            vector<int> &ops = ops_by_src_target[src][target];
            for (size_t op_no = 0; op_no < ops.size(); op_no++) {
                ou << "o_" << ops[op_no] <<
                   "(" << get_adjusted_action_cost(g_operators[ops[op_no]], cost_type) << "), ";
            }
            ou << "\"];" << endl;
        }
    }

    ou << "}" << endl;
}


std::ostream &operator << (std::ostream &ss, const Abstraction &abs)
{
    ss << "mas[";
    for (size_t i = 0; i < abs.varset.size(); i++) {
        ss << abs.varset[i] << " ";
    }
    return ss << "]";
}
////////////////////////////////////////////////////////////////////////////////////////
void Abstraction::dump_goal_leading_actions(bool /*backward_pruning*/) const
{
    std::cerr << "This function has not been ported yet..." << std::endl;
    assert(false);
    utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    // // Dumping to file
    // vector<bool> labels;
    // int num_labels = set_goal_leading_actions(labels, backward_pruning);
    // write_goal_leading_actions(labels, num_labels);

}

int Abstraction::set_goal_leading_actions(vector<bool> &labels,
        bool backward_pruning) const
{
    utils::Timer timer;

    cout << "Goal leading actions" << endl;

    assert(goal_distances.size() == (size_t) num_states);
    assert(transitions_by_op.size() == g_operators.size());
    assert(labels.size() == 0);
    labels.resize(g_operators.size(), false);
    int num_goal_leading = 0;
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int op_no = non_empty_transition->op_no;
        //for (int op_no = 0; op_no < g_operators.size(); op_no++) {
        const vector<AbstractTransition> &trans = transitions_by_op[op_no];
        int op_cost;
        if (last_computation_used_uniform_costs) {
            op_cost = 1;
        } else {
            op_cost = get_adjusted_action_cost(g_operators[op_no],
                                               cost_type);    // Peter: TODO Must do something about this!!!
        }
//        cout << "==========>Transitions on action no " << op_no <<
//        ". Number of transitions: " << trans.size() << endl;
        if (trans.size() == 0) {
//        	cout << "No transitions for operator" << endl;
//        	g_operators[op_no].dump();
            continue;
        }
//        g_operators[op_no].dump();
        for (size_t i = 0; i < trans.size(); i++) {
            int src = trans[i].src;
            int target = trans[i].target;
            // Check if the value of the target is less or equal the value of the source.
            int src_g = get_goal_distance(src);
            if ((src_g == infinity) ||
                    (init_state != PRUNED_STATE && backward_pruning
                     && src_g > get_goal_distance(init_state))) {
                continue;
            }

            int target_g = get_goal_distance(target);
//        	cout << "Transition from " << src << " (h*=" << src_g <<
//        	") to " << target << " (h*="<< target_g <<") with cost " << op_cost << endl;

            if (src_g == (target_g + op_cost)) {
#ifdef DEBUG_ABSTRACTIONS
                cout << "Goal leading: " << op_no << ":  ";
                g_operators[op_no].dump();
#endif
                labels[op_no] = true;
                num_goal_leading++;
                break;
            }
        }
        non_empty_transition = non_empty_transition->succ;
    }

    cout << "Dumped goal leading actions " << num_goal_leading <<
         " out of total number of actions " << g_operators.size() << endl;
    cout << "Finding label set took: " << timer << endl;

    return num_goal_leading;
}

int Abstraction::set_shortest_path_relevant_actions(vector<bool> &labels,
        bool backward_pruning)
{
    utils::Timer timer;
    cout << "Calculating shortest-path relevant actions" << endl;
    int num_actions = 0;

    bool recalculated_distances = false;

    if (!last_computation_used_uniform_costs) {
        // we need uniform cost actions to properly operate here, so we have to recalculate the distances accordingly
        compute_distances(false, false, true, true, true);
        recalculated_distances = true;
    }

    assert(goal_distances.size() == (size_t) num_states);
    assert(labels.size() == 0);
    labels.resize(g_operators.size(), false);

    //vector<vector<int> > outgoing_transitions(num_states);
    vector<set<int> > outgoing_transitions(num_states);
    vector<set<AbstractStateRef> > starting_states(g_operators.size());
    // calculate outgoing transitions for each state - disregarding those states with too high distance from goal
    for (size_t op_no = 0; op_no < g_operators.size(); op_no++) {
        const vector<AbstractTransition> &transitions = transitions_by_op[op_no];
        int op_cost = 1;
        for (size_t i = 0; i < transitions.size(); i++) {
            const AbstractStateRef &src = transitions[i].src;
            int src_g = get_goal_distance(src);
            if ((src_g == infinity) ||
                    (init_state != PRUNED_STATE && backward_pruning
                     && src_g > get_goal_distance(init_state))) {
                continue;
            }

            const AbstractStateRef &target = transitions[i].target;
            int target_g = get_goal_distance(target);

            if (src_g == (target_g + op_cost)) {
                //outgoing_transitions[src].push_back(op_no);
                outgoing_transitions[src].insert(op_no);
                starting_states[op_no].insert(src);
            }
        }
    }

    /*
     * Original idea: For each state, check if any outgoing action has been marked.
     * If not, mark the first one
     */
    /*for (int i = 0; i < num_states; i++) {
    	//::sort(outgoing_transitions[i].begin(), outgoing_transitions[i].end());
    	//vector<int>::iterator it = unique(outgoing_transitions[i].begin(), outgoing_transitions[i].end());
    	//outgoing_transitions[i].erase(it, outgoing_transitions[i].end());
    	if (!outgoing_transitions[i].empty()) {
    		bool some_action_caught = false;
    		vector<int> &transitions = outgoing_transitions[i];
    		for (int j = 0; j < transitions.size(); j++) {
    			if (labels[transitions[j]]) {
    				some_action_caught = true;
    				break;
    			}
    		}
    		if (!some_action_caught) {
    			labels[transitions[0]] = true;
    			num_actions++;
    		}
    	}
    }*/

    /*
     * New idea: Start with all operators that are strictly necessary, i.e., for which there is some state
     * that has only this as relevant outgoing transition. Then continue with those operators that are
     * relevant for the highest number of states.
     */
    mark_relevant_operators(num_states, outgoing_transitions, starting_states,
                            labels, num_actions);

    if (recalculated_distances) {
        // can be only because outside distances not computed by means of uniform-cost actions
        compute_distances(false, false, true, false, true);
    }

    cout << "Dumped shortest-path relevant actions " << num_actions <<
         " out of total number of actions " << g_operators.size() << endl;
    cout << "Finding label set took: " << timer << endl;

    return num_actions;
}

int Abstraction::set_path_relevant_actions(vector<bool> &labels,
        bool /*backward_pruning*/)
{
    utils::Timer timer;
    cout << "Calculating path relevant actions" << endl;
    int num_actions = 0;

    assert(goal_distances.size() == (size_t) num_states);
    assert(labels.size() == 0);
    labels.resize(g_operators.size(), false);





    vector<vector<pair<AbstractStateRef, int> > > backward_graph(num_states);
    vector<int> num_outgoing_actions(num_states, 0);
    vector<int> outgoing_action(num_states, -1);
    //vector<vector<int> > starting_states(g_operators.size());
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int op_no = non_empty_transition->op_no;
        //for (int op_no = 0; op_no < transitions_by_op.size(); op_no++) {
        const vector<AbstractTransition> &transitions = transitions_by_op[op_no];
        for (size_t j = 0; j < transitions.size(); j++) {
            const AbstractTransition &trans = transitions[j];
            backward_graph[trans.target].push_back(make_pair(trans.src, op_no));
            num_outgoing_actions[trans.src]++;
            outgoing_action[trans.src] = op_no;
            //starting_states[op_no].push_back(trans.src);
        }
        non_empty_transition = non_empty_transition->succ;
    }

    vector<bool> states_done(num_states, false);
    int num_states_done = 0;
    vector<bool> states_in_open_closed(num_states, false);
    //vector<bool> states_marked(num_states, false);
    list<AbstractStateRef> queue;
    for (int i = 0; i < num_states; i++) {
        if (goal_states[i]) {
            //cout << "done: " << i << endl;
            queue.push_back(i);
            states_done[i] = true;
            num_states_done++;
            states_in_open_closed[i] = true;
            //states_marked[i] = true;
        } else if (num_outgoing_actions[i] == 0) {
            // can only happen if we do not prune unreachable/irrelevant states
            states_done[i] = true;
            num_states_done++;
        } else if (num_outgoing_actions[i] == 1) {
            labels[outgoing_action[i]] = true;
            num_actions++;
        }
    }
    vector<int> ().swap(num_outgoing_actions);
    vector<int> ().swap(outgoing_action);

    vector<int> num_relevant_starting_states(g_operators.size(), 0);
    while (num_states_done != num_states) {
        //cout << "next iteration; " << num_states_done << " of " << num_states << " states already done." << endl;
        //cout << "                " << num_actions << " of " << g_operators.size() << " actions caught." << endl;
        // generate new states and update operators' marking counts
        list<int>::iterator queue_it = queue.begin();
        /*// go through queue once to keep only the undone states and the done states with undone predecessors*/
        // go through queue only once and update counts of operators on the fly (it is only an approximation anyway)
        while (queue_it != queue.end()) {
            int state = *queue_it;
            if (states_done[state]) {
                // some of state's outgoing operators are already caught; can insert all predecessors into queue
                bool all_preds_done = true;
                for (size_t i = 0; i < backward_graph[state].size(); i++) {
                    const pair<int, int> &pred_trans = backward_graph[state][i];
                    int pred = pred_trans.first;
                    if (!states_in_open_closed[pred]) {
                        states_in_open_closed[pred] = true;
                        queue.push_back(pred);
                    }
                    if (!states_done[pred]) {
                        int op_no = pred_trans.second;
                        if (labels[op_no]) {
                            states_done[pred] = true;
                            num_states_done++;
                        } else {
                            num_relevant_starting_states[op_no]++;
                            all_preds_done = false;
                        }
                    }
                }
                if (all_preds_done) {
                    // all predecessors are already done, so we can remove state from the queue
                    queue_it = queue.erase(queue_it);
                } else {
                    // not all predecessors done yet, so we have to keep state in the queue in order to properly update
                    // the counts of all operators
                    queue_it++;
                }
            } else {
                // no outgoing operators caught yet; must update operators' marking counts for this state
                /*for (int i = 0; i < backward_graph[state].size(); i++) {
                	const pair<int, int> &pred_trans = backward_graph[state][i];
                	int pred = pred_trans.first;
                	int op_no = pred_trans.second;
                	if (!states_done[pred]) {
                		num_relevant_starting_states[op_no]++;
                	}
                }*/
                queue_it++;
            }
        }

        if (num_states_done == num_states)
            // No need to catch another action -- we are already done
        {
            break;
        }

        /*queue_it = queue.begin();
        // go through queue a second time to get the correct counts for the operators
        while (queue_it != queue.end()) {
        	int state = *queue_it;
        	if (states_done[state]) {
        		// some of state's outgoing operators are already caught; can insert all predecessors into queue
        		bool all_preds_done = true;
        		for (int i = 0; i < backward_graph[state].size(); i++) {
        			const pair<int, int> &pred_trans = backward_graph[state][i];
        			int pred = pred_trans.first;
        			if (!states_in_open_closed[pred]) {
        				cerr << "This cannot happen!" << endl;
        				states_in_open_closed[pred] = true;
        				queue.push_back(pred);
        			}
        			if (!states_done[pred]) {
        				int op_no = pred_trans.second;
        				if (labels[op_no]) {
        					cerr << "This cannot happen!" << endl;
        					states_done[pred] = true;
        					num_states_done++;
        				} else {
        					num_relevant_starting_states[op_no]++;
        					all_preds_done = false;
        				}
        			}
        		}
        		if (all_preds_done) {
        			cerr << "This cannot happen!" << endl;
        			// all predecessors are already done, so we can remove state from the queue
        			queue_it = queue.erase(queue_it);
        		} else {
        			// not all predecessors done yet, so we have to keep state in the queue in order to properly update
        			// the counts of all operators
        			queue_it++;
        		}
        	} else {
        		// no outgoing operators caught yet; must update operators' marking counts for this state
        		queue_it++;
        	}
        }*/

        // catch operator with highest marking count
        int max_num = -1;
        int max_index = -1;
        for (size_t op_no = 0; op_no < g_operators.size(); op_no++) {
            if (num_relevant_starting_states[op_no] > max_num && !labels[op_no]) {
                max_num = num_relevant_starting_states[op_no];
                max_index = op_no;
            }
        }
        //cout << max_num << "; " << max_index << endl;
        labels[max_index] = true;
        num_relevant_starting_states.assign(g_operators.size(), 0);
        num_actions++;

        /*// push newly marked states into queue and update operators' marking counts
        const vector<AbstractTransition> &transitions = transitions_by_op[max_index];
        for (int i = 0; i < transitions.size(); i++) {
        	const AbstractTransition &trans = transitions[i];
        	//int target = trans.target;
        	//if (states_done[target]) {
        		int src = trans.src;
        		if (!states_marked[src]) {
        			states_marked[src] = true;
        			//num_states_done++;
        		}
        		if (!states_in_open_closed[src]) {
        			queue.push_back(src);
        			states_in_open_closed[src] = true;
        		}
        	//}
        }*/
    }

    /*while (!queue.empty()) {
        int state = queue.front();
        queue.pop_front();
        for (int i = 0; i < graph[state].size(); i++) {
            int successor = graph[state][i];
            if (!other_distances.empty() && other_distances[successor] == infinity) {
            	// state already proven to be irrelevant
            	continue;
            }
    		if (distances[successor] > distances[state] + 1) {
    			distances[successor] = distances[state] + 1;
    			queue.push_back(successor);
    		}
        }
    }*/





    /*vector<set<int> > outgoing_transitions(num_states);
    vector<set<int> > ingoing_transitions(num_states);
    vector<set<AbstractStateRef> > starting_states(g_operators.size());
    // calculate outgoing transitions for each state - disregarding those states with too high distance from goal
    for (int op_no = 0; op_no < g_operators.size(); op_no++) {
    	const vector<AbstractTransition> &transitions = transitions_by_op[op_no];
        int op_cost = 1;
    	for (int i = 0; i < transitions.size(); i++) {
    		const AbstractStateRef &src = transitions[i].src;
            int src_g = get_goal_distance(src);
            if ((src_g == infinity) *//*||
(init_state != PRUNED_STATE && backward_pruning && src_g > get_goal_distance(init_state))*//*) {
continue;
}

const AbstractStateRef &target = transitions[i].target;
int target_g = get_goal_distance(target);

outgoing_transitions[src].insert(op_no);
ingoing_transitions[target].insert(op_no);
starting_states[op_no].insert(src);
}
}

int num_states_done = 0;
vector<bool> states_done(num_states, false);
vector<bool> states_in_open_closed(num_states, false);
deque<int> open_list;
// Peter: TODO: What to do about goal states? So far, they are taken to start backward search, but do
// not need to have an outgoing action marked
for (int i = 0; i < num_states; i++) {
if (goal_states[i]) {
states_done[i] = true;
states_in_open_closed[i] = true;
num_states_done++;
open_list.push_back(i);
} else if (outgoing_transitions[i].empty()) {
// Peter: this can only happen if we do not prune unsolvable / irrelevant states
states_done[i] = true;
num_states_done++;
} else if (outgoing_transitions[i].size() == 1) {
// Peter: We don't care about outgoing actions of goal states. Plus, for those states having
// only a single outgoing action, we can immediately catch it.
states_done[i] = true;
num_states_done++;
int op_no = *(outgoing_transitions[i].begin());
labels[op_no] = true;
}
}

if (num_states_done < num_states) {
while (!open_list.empty()) {
const int state = open_list.front();
open_list.pop_front();
set<int>::const_iterator in_op_it;
for (in_op_it = ingoing_transitions.begin(); in_op_it != ingoing_transitions.end(); in_op_it++) {

}
}
}*/



    /*vector<set<int> > outgoing_actions(num_states);
    vector<set<int> > starting_states(g_operators.size());
    vector<int> num_starting_states(g_operators.size(), 0);
    int max_num = -1;
    //int max_index = -1;
    if (num_states_done < num_states) {
    	for (int op_no = 0; op_no < g_operators.size(); op_no++) {
    		const vector<AbstractTransition> &transitions = transitions_by_op[op_no];
    		for (int i = 0; i < transitions.size(); i++) {
    			const int target = transitions[i].target;
    			if (states_done[target]) {
    				const int src = transitions[i].src;
    				pair<set<int>::iterator, bool> result = starting_states[op_no].insert(src);
    				if (result.second) { // state was not in list before
    					num_starting_states[op_no]++;
    					if (num_starting_states[op_no] > max_num) {
    						max_num = num_starting_states[op_no];
    						//max_index = op_no;
    					}
    					outgoing_actions[src].insert(op_no);
    				}
    			}
    		}
    	}
    	while (num_states_done < num_states) {

    	}
    }*/

    /*//vector<vector<int> > outgoing_transitions(num_states);
    vector<set<int> > outgoing_transitions(num_states);
    vector<set<AbstractStateRef> > starting_states(g_operators.size());
    // calculate outgoing transitions for each state - disregarding those states with too high distance from goal
    for (int op_no = 0; op_no < g_operators.size(); op_no++) {
    	const vector<AbstractTransition> &transitions = transitions_by_op[op_no];
        int op_cost = 1;
    	for (int i = 0; i < transitions.size(); i++) {
    		const AbstractStateRef &src = transitions[i].src;
            int src_g = get_goal_distance(src);
            if ((src_g == infinity) ||
            		(init_state != PRUNED_STATE && backward_pruning && src_g > get_goal_distance(init_state))) {
            	continue;
            }

    		const AbstractStateRef &target = transitions[i].target;
            int target_g = get_goal_distance(target);

            if (src_g == (target_g + op_cost)) {
    			//outgoing_transitions[src].push_back(op_no);
    			outgoing_transitions[src].insert(op_no);
    			starting_states[op_no].insert(src);
            }
    	}
    }*/

    /*
     * Original idea: For each state, check if any outgoing action has been marked.
     * If not, mark the first one
     */
    /*for (int i = 0; i < num_states; i++) {
    	//::sort(outgoing_transitions[i].begin(), outgoing_transitions[i].end());
    	//vector<int>::iterator it = unique(outgoing_transitions[i].begin(), outgoing_transitions[i].end());
    	//outgoing_transitions[i].erase(it, outgoing_transitions[i].end());
    	if (!outgoing_transitions[i].empty()) {
    		bool some_action_caught = false;
    		vector<int> &transitions = outgoing_transitions[i];
    		for (int j = 0; j < transitions.size(); j++) {
    			if (labels[transitions[j]]) {
    				some_action_caught = true;
    				break;
    			}
    		}
    		if (!some_action_caught) {
    			labels[transitions[0]] = true;
    			num_actions++;
    		}
    	}
    }*/

    /*
     * New idea: Start with all operators that are strictly necessary, i.e., for which there is some state
     * that has only this as relevant outgoing transition. Then continue with those operators that are
     * relevant for the highest number of states.
     */
    //mark_relevant_operators(num_states, outgoing_transitions, starting_states, labels, num_actions);

    cout << "Dumped shortest-path relevant actions " << num_actions <<
         " out of total number of actions " << g_operators.size() << endl;
    cout << "Finding label set took: " << timer << endl;

    return num_actions;
}

void Abstraction::get_goal_leading_actions(set<int> &ops) const
{
    assert(goal_distances.size() == (size_t) num_states);
    assert(transitions_by_op.size() == g_operators.size());
    ArrayList *non_empty_transition = first_non_empty_transition;
    while (non_empty_transition) {
        int op_no = non_empty_transition->op_no;
        //for (int op_no = 0; op_no < g_operators.size(); op_no++) {
        const vector<AbstractTransition> &trans = transitions_by_op[op_no];
        int op_cost = get_adjusted_action_cost(g_operators[op_no], cost_type);
        for (size_t i = 0; i < trans.size(); i++) {
            int src = trans[i].src;
            int target = trans[i].target;
            // Check if the value of the target is less or equal the value of the source.
            int src_g = get_goal_distance(src);
            if (src_g == infinity) {
                continue;
            }
            if (get_goal_distance(src) == (get_goal_distance(target) + op_cost)) {
                ops.insert(op_no);
                break;
            }
        }
        non_empty_transition = non_empty_transition->succ;
    }
}

void Abstraction::write_mip_file(const bool backward_pruning,
                                 const bool use_uniform_costs)
{
    cout << "writing MIP file for finding outgoing actions in " <<
         (use_uniform_costs ? "unit-cost" : "original") << " setting..." << endl;
    if (!use_uniform_costs) {
        for (size_t i = 0; i < g_operators.size(); i++) {
            if (get_adjusted_action_cost(g_operators[i], cost_type) == 0) {
                cout << "zero-cost action present, not doing anything here!" << endl;
                return;
            }
        }
    }
    ofstream out(use_uniform_costs ? "necessary_actions_uniform.cplex" :
                 "necessary_actions.cplex");
    out << "Minimize" << endl;
    if (g_operators.size() > 0) {
        out << "   ";
        for (size_t i = 0; i < g_operators.size() - 1; i++) {
            out << "A" << i << " + ";
        }
        out << "A" << (g_operators.size() - 1) << endl;
    }
    out << "Subject To" << endl;
    vector<vector<int> > outgoing_transitions(num_states);
    for (size_t op_no = 0; op_no < g_operators.size(); op_no++) {
        const vector<AbstractTransition> &transitions = transitions_by_op[op_no];
        int op_cost = (use_uniform_costs ? 1 : get_adjusted_action_cost(
                           g_operators[op_no], cost_type));
        for (size_t i = 0; i < transitions.size(); i++) {
            const AbstractStateRef &src = transitions[i].src;
            int src_g = get_goal_distance(src);
            if ((src_g == infinity) ||
                    (init_state != PRUNED_STATE && backward_pruning
                     && src_g > get_goal_distance(init_state))) {
                continue;
            }

            const AbstractStateRef &target = transitions[i].target;
            int target_g = get_goal_distance(target);
//        	cout << "Transition from " << src << " (h*=" << src_g <<
//        	") to " << target << " (h*="<< target_g <<") with cost " << op_cost << endl;

            if (src_g == (target_g + op_cost)) {
                //if (goal_distances[src] < infinity - 1)
                outgoing_transitions[src].push_back(op_no);
            }
        }
    }
    for (int i = 0; i < num_states; i++) {
        ::sort(outgoing_transitions[i].begin(), outgoing_transitions[i].end());
        vector<int>::iterator it = unique(outgoing_transitions[i].begin(),
                                          outgoing_transitions[i].end());
        //adjacency_matrix[i].resize(distance(adjacency_matrix[i].begin(), it));
        outgoing_transitions[i].erase(it, outgoing_transitions[i].end());
        if (!outgoing_transitions[i].empty()) {
            out << "   S" << i << ": ";
            for (size_t j = 0; j < outgoing_transitions[i].size() - 1; j++) {
                out << "A" << outgoing_transitions[i][j] << " + ";
            }
            out << "A" << outgoing_transitions[i][outgoing_transitions[i].size() - 1] <<
                " >= 1" << endl;
        }
    }
    /*out << "Bounds" << endl;
    for (int i = 0; i < g_operators.size(); i++) {
    	out << "   T" << i << " <= 1" << endl;
    }*/
    out << "Binary" << endl;
    for (size_t i = 0; i < g_operators.size(); i++) {
        out << "   A" << i << endl;
    }
    out << "End" << endl;
    out.close();
    cout << "done" << endl;
}

void Abstraction::write_pmaxsat_file(const bool backward_pruning)
{
    cout << "writing partial MaxSAT file for finding actions preserving optimal solutions"
         << endl;
    compute_distances(false, true, true, false, true);
    /* params:
    * prune_unreachable
    * prune_irrelevant
    * continue_if_unsolvable
    * use_uniform_costs
    * force_recomputation
    */

    //if (backward_pruning) {
    // prune all states with distance infinity and, in case of backward_pruning, all with too big distance
    vector<std::list<AbstractStateRef> > remainingGlobalStates;
    int init_dist = get_goal_distance(init_state);
    for (int i = 0; i < num_states; i++) {
        int state_dist = get_goal_distance(i);
        if ((state_dist == infinity) ||
                (init_state != PRUNED_STATE && backward_pruning && state_dist > init_dist)) {
            continue;
        }
        std::list<AbstractStateRef> nextGlobalState;
        nextGlobalState.push_front(i);
        remainingGlobalStates.push_back(nextGlobalState);
    }
    apply_abstraction(remainingGlobalStates);
    // must recalculate distances, as abstraction was changed
    compute_distances(false, true, true, false, true);
    //}

    // remove suboptimal transitions and
    // store outgoing transition ids for each state
    vector<vector<int> > outgoing_transitions(num_states);
    int trans_index = 0;
    for (size_t op_no = 0; op_no < g_operators.size(); op_no++) {
        vector<AbstractTransition> &transitions = transitions_by_op[op_no];
        vector<AbstractTransition> new_transitions;
        int op_cost = get_adjusted_action_cost(g_operators[op_no], cost_type);
        for (size_t i = 0; i < transitions.size(); i++) {
            AbstractStateRef src = transitions[i].src;
            int src_g = get_goal_distance(src);
            // this is not needed anymore, as those states are already pruned
            /*if ((src_g == infinity) ||
            		(init_state != PRUNED_STATE && backward_pruning && src_g > get_goal_distance(init_state))) {
            	continue;
            }*/

            AbstractStateRef target = transitions[i].target;
            int target_g = get_goal_distance(target);
//        	cout << "Transition from " << src << " (h*=" << src_g <<
//        	") to " << target << " (h*="<< target_g <<") with cost " << op_cost << endl;

            if (src_g == (target_g + op_cost)) {
                //if (goal_distances[src] < infinity - 1)
                outgoing_transitions[src].push_back(trans_index);
                AbstractTransition new_trans(src, target);
                new_transitions.push_back(new_trans);
                trans_index++;
            }
        }
        transitions.swap(new_transitions);
        vector<AbstractTransition> ().swap(new_transitions);
    }
    cout << "reducing transitions done" << endl;

    // find maxDist by calculating abstraction based on uniform costs and checking for maximal h-value
    compute_distances(false, true, true, true, true);
    int maxDist = max_h;
    compute_distances(false, true, true, false, true);

    int offset_on_variables = 1;
    int num_on_variables = g_operators.size();
    int offset_r_variables = offset_on_variables + num_on_variables;
    int num_r_variables = (maxDist + 1) * num_states;
    int offset_t_variables = offset_r_variables + num_r_variables;
    int num_transitions = total_transitions();
    int num_t_variables = maxDist * num_transitions;
    int num_variables = num_on_variables + num_r_variables + num_t_variables;
    int num_on_clauses = num_on_variables;
    int num_r_clauses = num_r_variables + num_states;
    int num_t_clauses = 2 * num_t_variables;
    int num_clauses = num_on_clauses + num_r_clauses + num_t_clauses;
    int top = num_on_variables + 1;

    ofstream out("necessary_actions.wcnf");
    out << "c number of operators: " << num_on_variables << endl;
    out << "p wcnf " << num_variables << " " << num_clauses << " " << top << endl;

    // minimize the number of chosen operators
    int offset = offset_on_variables;
    for (int o = 0; o < num_on_variables; o++) {
        out << "1 -" << (offset + o) << " 0" << endl;
    }

    // require last iteration's states to be true
    offset = offset_r_variables + maxDist * num_states;
    for (int s = 0; s < num_states; s++) {
        out << top << " " << (offset + s) << " 0" << endl;
    }

    // set first iteration's states to true/false dependent on their goal status
    offset = offset_r_variables;
    for (int s = 0; s < num_states; s++) {
        out << top << " " << (goal_states[s] ? "" : "-") << (offset + s) << " 0" <<
            endl;
    }

    // set all but the first iteration's states based on their outgoing transitions
    offset = offset_r_variables;
    for (int s = 0; s < num_states; s++) {
        for (int k = 1; k <= maxDist; k++) {
            out << top << " -" << (offset + k * num_states + s);
            out << " " << (offset + (k - 1) * num_states + s);
            const vector<int> &transitions = outgoing_transitions[s];
            for (size_t t = 0; t < transitions.size(); t++) {
                out  << " " << (offset_t_variables + (k - 1) * num_transitions +
                                transitions[t]);
            }
            out << " 0" << endl;
        }
    }

    // set transitions based on their target value from the previous transition and the value of the corresponding operator
    offset = offset_t_variables;
    trans_index = 0;
    for (size_t o = 0; o < g_operators.size(); o++) {
        const vector<AbstractTransition> &transitions = transitions_by_op[o];
        for (size_t i = 0; i < transitions.size(); i++) {
            const AbstractStateRef &target = transitions[i].target;
            for (int k = 1; k <= maxDist; k++) {
                out << top << " -" << (offset + (k - 1) * num_transitions + trans_index);
                out << " " << offset_r_variables + (k - 1) * num_states + target;
                out << " 0" << endl;
                out << top << " -" << (offset + (k - 1) * num_transitions + trans_index);
                out << " " << (offset_on_variables + o);
                out << " 0" << endl;
            }
            trans_index++;
        }
    }
    out.close();
    cout << "done" << endl;
}

void Abstraction::write_action_files(const bool backward_pruning)
{
    compute_distances(false, true, true, false, true);
    dump_goal_leading_actions(backward_pruning);
    write_mip_file(backward_pruning, false);
    compute_distances(false, true, true, true, true);
    write_mip_file(backward_pruning, true);
    write_pmaxsat_file(backward_pruning);
}




// void Abstraction::count_transitions_var_empty(vector<double> & count) const {
//   for(int i = 0; i < vars_label_operators.size(); ++i){
//     if(vars_label_operators[i].size() == 1 && !transitions_by_op[i].empty() ){
//       int var = *(vars_label_operators[i].begin());
//       for(int j = 0; j < transitions_by_op[i].size(); j++){
// 	if(transitions_by_op[i][j].target != transitions_by_op[i][j].src){
// 	  count[var] ++;
// 	}
//       }
//     }
//   }
// }

// void Abstraction::count_transitions_var_empty_goal(vector<double> & count) const {
//   for(int i = 0; i < vars_label_operators.size(); ++i){
//     if(vars_label_operators[i].size() == 1){
//       int var = *(vars_label_operators[i].begin());
//       for(int j = 0; j < transitions_by_op[i].size(); j++){
// 	if(transitions_by_op[i][j].target != transitions_by_op[i][j].src &&
// 	   goal_states[transitions_by_op[i][j].target]){
// 	  count[var] ++;
// 	}
//       }
//     }
//   }
// }

// void Abstraction::count_transitions_var(vector<double> & count) const {
//   for(int i = 0; i < vars_label_operators.size(); ++i){
//     if(transitions_by_op[i].empty()) continue;
//     int num_transitions = 0;
//     for(int j = 0; j < transitions_by_op[i].size(); j++){
//       if(transitions_by_op[i][j].target != transitions_by_op[i][j].src){
// 	num_transitions ++;
//       }
//     }
//     for(set<int>::iterator it = vars_label_operators[i].begin();
// 	it != vars_label_operators[i].end(); ++it){
//       count[*it] += num_transitions;
//     }
//   }
// }

void Abstraction::count_transitions()
{
    number_transitions_var.resize(g_variable_domain.size(), 0);
    number_transitions_var_empty.resize(g_variable_domain.size(), 0);
    number_transitions_var_empty_goal.resize(g_variable_domain.size(), 0);

    for (size_t i = 0; i < vars_label_operators.size(); ++i) {
        if (transitions_by_op[i].empty()) {
            continue;
        }
        int num_transitions = 0;
        int num_transitions_goal = 0;
        for (size_t j = 0; j < transitions_by_op[i].size(); j++) {
            if (transitions_by_op[i][j].target != transitions_by_op[i][j].src) {
                num_transitions ++;
                if (goal_states[transitions_by_op[i][j].target]) {
                    num_transitions_goal ++;
                }
            }
        }
        for (set<int>::iterator it = vars_label_operators[i].begin();
                it != vars_label_operators[i].end(); ++it) {
            number_transitions_var[*it] += num_transitions;
        }
        if (vars_label_operators[i].size() == 1) {
            int var = *(vars_label_operators[i].begin());
            number_transitions_var_empty[var] += num_transitions;
            number_transitions_var_empty_goal[var] += num_transitions_goal;
        }
    }
}

}
