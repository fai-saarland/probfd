#include "abstract_solution_data.h"

#include "pattern_collection_generator_cegar.h" // TODO: only for Verbosity

#include "../successor_generator.h"
#include "../global_state.h"

#include "../utils/hash.h"
#include "../utils/rng.h"

#include <functional>
#include <limits>
#include <queue>

using namespace std;
using utils::Verbosity;

namespace pdbs {

static void dump_state_sequence(const vector<AbstractState> &state_sequence) {
    string prefix = "";
    for (const AbstractState &state : state_sequence) {
        cout << prefix << state.get_values();
        prefix = ", ";
    }
    cout << endl;
}




AbstractSolutionData::AbstractSolutionData(
        OperatorCost cost_type,
        const Pattern& pattern,
        set<int> blacklist,
        const shared_ptr<utils::RandomNumberGenerator>& rng,
        bool extended,
        Verbosity verbosity)
        : pdb(new PatternDatabase(pattern, cost_type)),
          abstracted_task(pattern),
          abs_task_proxy(abstracted_task),
          blacklist(std::move(blacklist)),
          generate_extended(extended),
          is_solvable(true),
          solved(false)
{
    if (verbosity >= Verbosity::VERBOSE) {
        cout << "Computing plan for PDB with pattern "
             << get_pattern() << endl;
    }

    const auto initial_values = abstracted_task.get_initial_state_values();
    if (pdb->get_value_abstracted(initial_values) == numeric_limits<int>::max())
    {
        is_solvable = false;
        if (verbosity >= Verbosity::VERBOSE) {
            cout << "PDB with pattern " << get_pattern()
                 << " is unsolvable" << endl;
        }
        return;
    }

    auto succ_gen = abstracted_task.get_successor_generator();
    vector<AbstractState> state_sequence =
        steepest_ascent_enforced_hillclimbing(*succ_gen, rng, verbosity);
    turn_state_sequence_into_plan(*succ_gen, rng, verbosity, state_sequence);
}

vector<AbstractState>
AbstractSolutionData::extract_state_sequence(const SearchNode &goal_node) const
{
    vector<AbstractState> state_sequence;
    // Handle first state outside the loop because we need to copy.
    state_sequence.push_back(goal_node.state);

    shared_ptr<SearchNode> current_node = goal_node.predecessor;
    while (true) {
        state_sequence.push_back(move(current_node->state));
        if (current_node->predecessor) {
            assert(current_node->cost != -1);
            current_node = current_node->predecessor;
        } else {
            reverse(state_sequence.begin(), state_sequence.end());
            return state_sequence;
        }
    }
}

vector<AbstractState>
AbstractSolutionData::bfs_for_improving_state(
    const successor_generator::SuccessorGenerator<int> &succ_gen,
    const shared_ptr<utils::RandomNumberGenerator> &rng,
    const int f_star,
    shared_ptr<SearchNode> &start_node) const
{
    // cout << "Running BFS with start state "
    //      << start_node->state.get_values() << endl;
    assert(start_node->cost == -1);
    assert(start_node->predecessor == nullptr);
    queue<shared_ptr<SearchNode>> open;
    utils::HashSet<size_t> closed;
    closed.insert(start_node->hash);
    int h_start = pdb->get_value_for_index(start_node->hash);
    open.push(move(start_node));
    while (true) {
        shared_ptr<SearchNode> current_node = open.front();
        open.pop();

        vector<int> applicable_ops;
        succ_gen.generate_applicable_ops(
            current_node->state.get_values(), applicable_ops);
        rng->shuffle(applicable_ops);

        shared_ptr<SearchNode> best_improving_succ_node;
        int best_h_so_far = h_start;

        for (int op_id : applicable_ops) {
            OperatorProxy op = abs_task_proxy.get_operators()[op_id];
            // cout << "applying op " << op.get_name() << endl;
            AbstractState successor = current_node->state.get_successor(op);
            size_t successor_index = pdb->get_abstract_state_index(
                successor.get_values());
            if (!utils::contains(closed, successor_index)) {
                int h_succ = pdb->get_value_for_index(successor_index);
                int op_cost = op.get_cost();
                int g_succ = current_node->g + op_cost;
                int f_succ = g_succ + h_succ;

                if (f_succ == f_star) {
                    //cout << "f* successor: " << successor.get_values()
                    //     << endl;
                    shared_ptr<SearchNode> succ_node =
                        make_shared<SearchNode>(
                            move(successor), successor_index, g_succ, op_cost, current_node);
                    assert(!utils::contains(closed, successor_index));

                    if (abstracted_task.is_goal_state(
                        succ_node->state.get_values()))
                    {
                        h_succ = -1;
                    }
                    if (h_succ < best_h_so_far) {
                        best_h_so_far = h_succ;
                        best_improving_succ_node = succ_node;
                    }

                    closed.insert(successor_index);
                    open.push(succ_node);
                }
            }
        }

        if (best_improving_succ_node) {
            start_node = best_improving_succ_node;
            return extract_state_sequence(*best_improving_succ_node);
        }
    }
}

vector<AbstractState> AbstractSolutionData::steepest_ascent_enforced_hillclimbing(
    const successor_generator::SuccessorGenerator<int> &succ_gen,
    const shared_ptr<utils::RandomNumberGenerator> &rng,
    Verbosity verbosity) const
{
    AbstractState start = abs_task_proxy.get_initial_state();
    size_t start_index = pdb->get_abstract_state_index(start.get_values());
    const int f_star = pdb->get_value_for_index(start_index);
    vector<AbstractState> state_sequence;
    state_sequence.push_back(start);

    if (verbosity >= Verbosity::VERBOSE) {
        cout << "Running EHC with start state " << start.get_values() << endl;
    }

    shared_ptr<SearchNode> start_node
        = make_shared<SearchNode>(move(start), start_index, 0, -1, nullptr);
    
    while (!abstracted_task.is_goal_state(start_node->state.get_values())) {
        if (verbosity >= Verbosity::VERBOSE) {
            cout << "Current start state of iteration: " << start_node->state.get_values() << endl;
        }

        // Reset start node to start a new BFS in this iteration.
        start_node->cost = -1;
        start_node->predecessor = nullptr;

        // start_node will be set to the last node of the BFS, thus containing
        // the improving state for the next iteration, and the updated g-value.
        vector<AbstractState> bfs_state_sequence =
            bfs_for_improving_state(succ_gen, rng, f_star, start_node);
        
        if (verbosity >= Verbosity::VERBOSE) {
            cout << "BFS state sequence to next improving state (adding all except first one): ";
            dump_state_sequence(bfs_state_sequence);
        }

        /* We do not add the first state since this equals the last state of
           the previous call to BFS, or the initial state, which we add
           manually. */
        for (size_t i = 1; i < bfs_state_sequence.size(); ++i) {
            AbstractState &state = bfs_state_sequence[i];
            state_sequence.push_back(move(state));
        }
    }

    if (verbosity >= Verbosity::VERBOSE) {
        cout << "EHC terminated. State sequence: ";
        dump_state_sequence(state_sequence);
    }

    return state_sequence;
}

void AbstractSolutionData::turn_state_sequence_into_plan(
    const successor_generator::SuccessorGenerator<int> &succ_gen,
    const shared_ptr<utils::RandomNumberGenerator> &rng,
    Verbosity verbosity,
    const vector<AbstractState> &state_sequence)
{
    extended_plan.reserve(state_sequence.size() - 1);

    if (verbosity >= Verbosity::VERBOSE) {
        cout << "Turning state seqeuence into plan..." << endl;
    }

    for (size_t i = 0; i < state_sequence.size() - 1; ++i) {
        const AbstractState &state = state_sequence[i];
        const AbstractState &successor = state_sequence[i + 1];
        // cout << "transition from " << state.get_values() << "to "
        //      << successor.get_values() << endl;
        assert(successor != state);
        int best_cost = numeric_limits<int>::max();
        vector<int> applicable_ops;
        succ_gen.generate_applicable_ops(state.get_values(), applicable_ops);
        vector<int> equivalent_ops;
        for (int op_id : applicable_ops) {
            OperatorProxy op = abs_task_proxy.get_operators()[op_id];
            // cout << "consider op " << op.get_name() << endl;
            int op_cost = op.get_cost();
            if (op_cost > best_cost) {
                // cout << "too expensive" << endl;
                continue;
            }

            AbstractState succ = state.get_successor(op);
            if (succ == successor) {
                // cout << "leads to right successor" << endl;
                if (op_cost < best_cost) {
                    // cout << "new best cost, updating" << endl;
                    equivalent_ops.clear();
                    best_cost = op_cost;
                }
                assert(op_cost == best_cost);
                equivalent_ops.push_back(op_id);
            }
        }

        if (generate_extended) {
            extended_plan.push_back(equivalent_ops);
        } else {
            int random_op_id = *rng->choose(equivalent_ops);
            extended_plan.emplace_back();
            extended_plan.back().push_back(random_op_id);
        }
    }

    if (verbosity >= Verbosity::VERBOSE) {
        print_plan(std::cout);
    }
}

const Pattern& AbstractSolutionData::get_pattern() const
{
    return pdb->get_pattern();
}

void AbstractSolutionData::blacklist_variable(int var)
{
    blacklist.insert(var);
}

bool AbstractSolutionData::is_blacklisted(int var) const
{
    return utils::contains(blacklist, var);
}

const std::set<int>& AbstractSolutionData::get_blacklist() const
{
    return blacklist;
}

const PatternDatabase&
AbstractSolutionData::get_pdb() const
{
    assert(pdb);
    return *pdb;
}

std::unique_ptr<PatternDatabase>
AbstractSolutionData::steal_pdb() {
    return std::move(pdb);
}

bool AbstractSolutionData::solution_exists() const
{
    return is_solvable;
}

void AbstractSolutionData::mark_as_solved()
{
    solved = true;
}

bool AbstractSolutionData::is_solved() const
{
    return solved;
}

const std::vector<std::vector<int>>&
AbstractSolutionData::get_plan() const
{
    return extended_plan;
}

int
AbstractSolutionData::convert_operator_index_to_parent(int abs_op_id) const
{
    OperatorProxy abs_op = abs_task_proxy.get_operators()[abs_op_id];
    return abs_op.convert_operator_index_to_parent();
}

int AbstractSolutionData::compute_plan_cost() const {
    int sum_cost = 0;
    for (auto step : extended_plan) {
        // all equivalent ops have the same cost, so we just take the first op
        sum_cost += abs_task_proxy.get_operators()[step[0]].get_cost();
    }
    return sum_cost;
}

void AbstractSolutionData::print_plan(std::ostream& out) const {
    out << "##### Plan for pattern " << get_pattern() << " #####" << endl;
    int i = 1;
    for (const auto &eqv_ops : extended_plan) {
        out << "step #" << i << endl;
        for (int opid : eqv_ops) {
            OperatorProxy op = abs_task_proxy.get_operators()[opid];
            out << op.get_name() << " " << op.get_cost() << endl;
        }
        ++i;
    }
    out << "##### End of plan #####" << endl;
}

}
