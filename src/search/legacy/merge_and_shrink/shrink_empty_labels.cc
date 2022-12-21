#include "legacy/merge_and_shrink/shrink_empty_labels.h"

#include "legacy/merge_and_shrink/abstraction.h"

#include "legacy/global_operator.h"
#include "legacy/globals.h"

#include "option_parser.h"
#include "plugin.h"

#include <cassert>
#include <iostream>
#include <limits>

namespace legacy {
namespace merge_and_shrink {

inline int get_op_index(const GlobalOperator* op)
{
    /* TODO: The op_index computation is duplicated from
     LabelReducer::get_op_index() and actually belongs neither
     here nor there. There should be some canonical way of getting
     from an GlobalOperator pointer to an index, but it's not clear how to
     do this in a way that best fits the overall planner
     architecture (taking into account axioms etc.) */
    int op_index = op - &*g_operators.begin();
    assert(op_index >= 0 && (size_t)op_index < g_operators.size());
    return op_index;
}

ShrinkEmptyLabels::ShrinkEmptyLabels(const options::Options& opts)
    : ShrinkStrategy(opts)
{
}

ShrinkEmptyLabels::~ShrinkEmptyLabels()
{
}

string ShrinkEmptyLabels::name() const
{
    return "empty labels (to identify unsol. tasks)";
}

void ShrinkEmptyLabels::dump_strategy_specific_options() const
{
}

bool ShrinkEmptyLabels::reduce_labels_before_shrinking() const
{
    return true;
}

void ShrinkEmptyLabels::shrink(Abstraction& abs, int target, bool /*force*/)
{
    /*
     * apply two rules:
     * (1) aggregate all states s1, ..., sn if they lie on an empty-label cycle
     *     idea: use Tarjan's algorithm to identify strongly connected
     * components (SCCs) (2) aggregate state s with goal state g if (a) all goal
     * variables are in abstraction and (b) there is an empty-label path from s
     * to g idea: set goal-status for all states already during SCC detection
     *     PETER: why only aggregate s with g and not all states that are marked
     * as goal states? PETER: when we are at it, why not remove all outgoing
     * transitions of goal states (we cannot leave them anyway any more)? PETER:
     * actually, if we use the first optimization, we must use the second one as
     * well, as otherwise unreachable states might suddenly become reachable
     * resulting in an immense overhead in abstract states PETER: we might
     * actually do this as a general optimization in Abstraction::normalize:
     * whenever all goal variables are merged in, we can safely remove any
     * transitions starting at goal states
     */

    vector<bool> empty_label_operators;
    determine_empty_label_operators(abs, empty_label_operators);

    /*bool all_goal_vars_in = true;
    for (size_t g = 0; g < g_goal.size(); g++) {
        if (!abs.is_in_varset(g_goal[g].first)) {
                all_goal_vars_in = false;
                break;
        }
    }*/
    int num_states = abs.size();
    vector<bool> is_goal;
    if (abs.get_all_goal_vars_in()) {
        is_goal.resize(num_states);
        for (int i = 0; i < num_states; i++) {
            is_goal[i] = abs.is_goal_state(i);
        }
    }

    /* this is a rather memory-inefficient way of implementing Tarjan's
     * algorithm, but it's the best I got for now */
    vector<vector<int>> adjacency_matrix(num_states);
    const vector<const GlobalOperator*>& relevant_ops =
        abs.get_relevant_operators();
    for (size_t op_no = 0; op_no < relevant_ops.size(); op_no++) {
        if (!empty_label_operators[op_no]) {
            continue;
        }
        const vector<AbstractTransition>& transitions =
            abs.get_transitions_for_op(get_op_index(relevant_ops[op_no]));
        for (size_t t = 0; t < transitions.size(); t++) {
            const AbstractTransition& trans = transitions[t];
            adjacency_matrix[trans.src].push_back(trans.target);
        }
    }

    /* free some memory */
    vector<bool>().swap(empty_label_operators);

    /* PETER: Can we do better than this, i.e., prevent the sorting? */
    /* remove duplicates in adjacency matrix */
    for (int i = 0; i < num_states; i++) {
        ::sort(adjacency_matrix[i].begin(), adjacency_matrix[i].end());
        vector<int>::iterator it =
            unique(adjacency_matrix[i].begin(), adjacency_matrix[i].end());
        // adjacency_matrix[i].resize(distance(adjacency_matrix[i].begin(),
        // it));
        adjacency_matrix[i].erase(it, adjacency_matrix[i].end());
    }
    /*for (int i = 0; i < num_states; i++) {
        cout << "edges from " << i << " to";
        for (int j = 0; j < adjacency_matrix[i].size(); j++)
                cout << " " << adjacency_matrix[i][j];
        cout << endl;
    }
    cout << endl;*/

    /* perform Tarjan's algorithm for finding SCCs */
    vector<int> indices(num_states, -1);
    vector<int> lowlinks(num_states, -1);
    vector<bool> in_stack(num_states, false);
    int index = 0;
    vector<int> stack;
    EquivalenceRelation final_sccs;
    vector<int> state_to_scc(num_states, -1);
    for (int i = 0; i < num_states; i++) {
        if (indices[i] == -1)
        // find_sccs: defined in abstraction
        {
            merge_and_shrink::find_sccs(
                stack,
                in_stack,
                final_sccs,
                adjacency_matrix,
                indices,
                lowlinks,
                index,
                i,
                abs.get_all_goal_vars_in(),
                is_goal,
                state_to_scc);
        }
    }

    /* free some memory */
    vector<int>().swap(indices);
    vector<int>().swap(lowlinks);
    vector<bool>().swap(in_stack);

    /*cout << "===========================================" << endl;
    for (int i = 0; i < num_states; i++) {
        cout << "edges from " << i << " to";
        for (int j = 0; j < adjacency_matrix[i].size(); j++)
                cout << " " << adjacency_matrix[i][j];
        cout << endl;
    }
    cout << "found SCCs:" << endl;
    for (int i = 0; i < final_sccs.size(); i++) {
        for (int j = 0; j < final_sccs[i].size(); j++)
                cout << final_sccs[i][j] << " ";
        cout << endl;
    }
    cout << "===========================================" << endl;*/

    int new_size = final_sccs.size();
    if (abs.get_all_goal_vars_in()) {
        /* now bring those groups together that follow the second rule */
        cout << "also using second rule of empty-label shrinking" << endl;
#define NEW_SECOND_STEP
#ifdef NEW_SECOND_STEP
        int goal_scc = -1;
        for (size_t i = 0; i < final_sccs.size(); i++) {
            if (is_goal[final_sccs[i].front()]) {
                if (goal_scc == -1) {
                    goal_scc = i;
                } else {
                    final_sccs[goal_scc].splice(
                        final_sccs[goal_scc].end(),
                        final_sccs[i]);
                    new_size--;
                }
            }
        }

#else
        vector<int> scc_position(new_size);
        for (int i = 0; i < new_size; i++) {
            scc_position[i] = i;
        }
        for (int i = 0; i < final_sccs.size(); i++) {
            EquivalenceClass& final_scc = final_sccs[i];
            if (final_scc.empty() || !is_goal[*final_scc.begin()]) {
                /* no need to investigate this group further:
                 *  - group already empty, or
                 *  - no connection to goal states exists
                 */
                continue;
            }
            EquivalenceClass::iterator scc_it = final_scc.begin();
            for (; scc_it != final_scc.end(); scc_it++) {
                int s = *scc_it;
                vector<int>& successors = adjacency_matrix[s];
                for (int j = 0; j < successors.size(); j++) {
                    int succ = successors[j];
                    if (!is_goal[succ]) {
                        /* no need to consider this state further: is no goal */
                        continue;
                    }
                    int succ_scc = state_to_scc[succ];
                    vector<int> seen_sccs;
                    seen_sccs.push_back(succ_scc);
                    while (scc_position[succ_scc] != succ_scc) {
                        succ_scc = scc_position[succ_scc];
                        seen_sccs.push_back(succ_scc);
                    }
                    for (int k = 0; k < seen_sccs.size(); k++) {
                        // update positions of all seen SCCs
                        scc_position[seen_sccs[k]] = i;
                    }
                    if (succ_scc == i) {
                        // in same group already
                        continue;
                    }
                    if (succ_scc < i)
                    /* add successors before current state: they were already
                       handled */
                    {
                        final_scc.splice(scc_it, final_sccs[succ_scc]);
                    } else
                    /* add successors at end of current list: they still need to
                       be handled */
                    {
                        final_scc.splice(final_scc.end(), final_sccs[succ_scc]);
                    }
                    new_size--;
                    if (new_size == 1) {
                        break;
                    }
                }
                if (new_size == 1) {
                    break;
                }
            }
            if (new_size == 1) {
                break;
            }
        }
        /* free some memory */
        vector<int>().swap(scc_position);
#endif
    }
    /* free some memory */
    vector<vector<int>>().swap(adjacency_matrix);
    vector<bool>().swap(is_goal);
    vector<int>().swap(state_to_scc);

    if (new_size < num_states) {
        // only need to apply abstraction if this actually changes anything
        EquivalenceRelation equivalence_relation(new_size);
        int counter = 0;
        for (size_t group = 0; group < final_sccs.size(); ++group) {
            if (final_sccs[group].empty()) {
                continue;
            }
            equivalence_relation[counter].swap(final_sccs[group]);
            counter++;
        }
        apply(abs, equivalence_relation, target);
        /*#ifdef NEW_SECOND_STEP
                        if (all_goal_vars_in)
                                abs.remove_goal_outgoing_transitions();
        #endif*/

        // vector<int> ().swap(state_to_group);
        EquivalenceRelation().swap(equivalence_relation);
    } else {
        cout << "Empty-label shrinking does not reduce states" << endl;
    }
    return;
}

void ShrinkEmptyLabels::shrink_atomic(Abstraction& abs)
{
    shrink(abs, abs.size(), true);
}

void ShrinkEmptyLabels::shrink_before_merge(
    Abstraction& abs1,
    Abstraction& abs2)
{
    /*pair<int, int> new_sizes = compute_shrink_sizes(abs1.size(), abs2.size());
    shrink(abs1, new_sizes.first, true);
    shrink(abs2, new_sizes.second, true);*/
    shrink(abs1, abs1.size(), true);
    shrink(abs2, abs2.size(), true);
    return;
}

void ShrinkEmptyLabels::determine_empty_label_operators(
    Abstraction& abs,
    vector<bool>& empty_label_operators)
{
    assert(empty_label_operators.empty());

    const vector<const GlobalOperator*>& relevant_ops =
        abs.get_relevant_operators();
    empty_label_operators.resize(relevant_ops.size(), true);

    const vector<int>& abstraction_vars = abs.get_varset();
    vector<bool> is_abstraction_var;
    is_abstraction_var.resize(g_variable_name.size(), false);
    for (size_t v = 0; v < abstraction_vars.size(); v++) {
        is_abstraction_var[abstraction_vars[v]] = true;
    }

    /*
     * go through all relevant operators
     * if some variable is in pre or eff that is not in abstraction it is not an
     * empty label operator
     */
    for (size_t op_no = 0; op_no < relevant_ops.size(); ++op_no) {
        const GlobalOperator* op = relevant_ops[op_no];
        const vector<GlobalCondition>& prev = op->get_preconditions();
        for (size_t i = 0; i < prev.size(); ++i) {
            int var = prev[i].var;
            if (!is_abstraction_var[var]) {
                empty_label_operators[op_no] = false;
                break;
            }
        }
        if (empty_label_operators[op_no]) {
            const vector<GlobalEffect>& pre_post = op->get_effects();
            for (size_t i = 0; i < pre_post.size(); ++i) {
                int var = pre_post[i].var;
                if (!is_abstraction_var[var]) {
                    empty_label_operators[op_no] = false;
                    break;
                }
            }
        }
    }
}

std::shared_ptr<ShrinkEmptyLabels> ShrinkEmptyLabels::create_default()
{
    const int infinity = numeric_limits<int>::max();
    options::Options opts;
    opts.set<int>("max_states", infinity);
    opts.set<int>("max_states_before_merge", infinity);
    return std::make_shared<ShrinkEmptyLabels>(opts);
}

static std::shared_ptr<ShrinkStrategy> _parse(options::OptionParser& parser)
{
    ShrinkStrategy::add_options_to_parser(parser);

    options::Options opts = parser.parse();

    if (!parser.dry_run()) {
        ShrinkStrategy::handle_option_defaults(opts);
        return std::make_shared<ShrinkEmptyLabels>(opts);
    } else {
        return 0;
    }
}

static Plugin<ShrinkStrategy> _plugin("legacy_shrink_empty_labels", _parse);

} // namespace merge_and_shrink
} // namespace legacy