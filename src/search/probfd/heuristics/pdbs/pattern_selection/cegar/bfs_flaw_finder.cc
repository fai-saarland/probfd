#include "probfd/heuristics/pdbs/pattern_selection/cegar/bfs_flaw_finder.h"

#include "probfd/heuristics/pdbs/pattern_selection/cegar/abstract_solution_data.h"
#include "probfd/heuristics/pdbs/pattern_selection/cegar/pattern_collection_generator_cegar.h"

#include "probfd/heuristics/pdbs/expcost_projection.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"

#include "option_parser.h"
#include "plugin.h"

#include "utils/collections.h"

#include "task_utils/task_properties.h"

using namespace std;
using namespace utils;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
BFSFlawFinder<PDBType>::BFSFlawFinder(
    const ProbabilisticTask* task,
    unsigned int violation_threshold)
    : FlawFindingStrategy<PDBType>(task)
    , closed(
          16,
          typename FlawFindingStrategy<PDBType>::StateHash(this->task_proxy))
    , violation_threshold(violation_threshold)
{
}

template <typename PDBType>
std::pair<FlawList, bool> BFSFlawFinder<PDBType>::apply_policy(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    std::vector<int>& init)
{
    assert(open.empty() && closed.empty());

    FlawList flaw_list;
    bool executable = true;
    unsigned int violations = 0;

    open.push_back(init);
    closed.insert(init);

    while (!open.empty()) {
        std::vector<int> current = open.front();
        open.pop_front();

        bool successful = expand(base, solution_index, current, flaw_list);
        executable = executable && successful;

        if (!successful && ++violations >= violation_threshold) {
            open.clear();
            break;
        }
    }

    closed.clear();
    return {flaw_list, executable};
}

template <typename PDBType>
bool BFSFlawFinder<PDBType>::expand(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    std::vector<int>& state,
    FlawList& flaw_list)
{
    AbstractSolutionData<PDBType>& solution = *base.solutions[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const PDBType& pdb = solution.get_pdb();

    // Check flaws, generate successors
    const StateRank abs = pdb.get_abstract_state(state);

    // We reached a dead-end, the operator is irrelevant.
    if (pdb.is_dead_end(abs)) {
        return true;
    }

    const auto& abs_operators = policy[abs];

    // We reached a terminal state, check if it is a goal
    if (abs_operators.empty()) {
        assert(pdb.is_goal(abs));

        State terminal_state(*this->task, move(state));

        if (pdb.is_goal(abs) &&
            !task_properties::is_goal_state(this->task_proxy, terminal_state)) {
            if (!base.ignore_goal_violations) {
                // Collect all non-satisfied goal variables that are still
                // available.
                for (FactProxy fact : this->task_proxy.get_goals()) {
                    const int goal_var = fact.get_variable().get_id();
                    const int goal_val = fact.get_value();

                    if (terminal_state[goal_var].get_value() != goal_val &&
                        !utils::contains(base.global_blacklist, goal_var) &&
                        utils::contains(base.remaining_goals, goal_var)) {
                        flaw_list.emplace_back(true, solution_index, goal_var);
                    }
                }
            }

            return false;
        }

        return true;
    }

    FlawList local_flaws;

    const ProbabilisticOperatorsProxy operators =
        this->task_proxy.get_operators();

    for (const AbstractOperator* abs_op : abs_operators) {
        int original_id = abs_op->original_operator_id;
        ProbabilisticOperatorProxy op = operators[original_id];

        // Check whether all preconditions are fulfilled
        bool preconditions_ok = true;

        for (const FactProxy fact : op.get_preconditions()) {
            const int pre_var = fact.get_variable().get_id();
            const int pre_val = fact.get_value();

            // We ignore blacklisted variables
            const bool is_blacklist_var =
                utils::contains(base.global_blacklist, pre_var);

            if (is_blacklist_var || solution.is_blacklisted(pre_var)) {
                assert(
                    !solution.is_blacklisted(pre_var) ||
                    base.local_blacklisting);
                continue;
            }

            if (state[pre_var] != pre_val) {
                preconditions_ok = false;
                local_flaws.emplace_back(false, solution_index, pre_var);
            }
        }

        // Flaws occured.
        if (!preconditions_ok) {
            continue; // Try next operator
        }

        // Generate the successors and add them to the open list
        for (const ProbabilisticOutcomeProxy outcome : op.get_outcomes()) {
            std::vector<int> successor =
                this->apply_op_to_state(state, outcome);

            if (!utils::contains(closed, successor)) {
                closed.insert(successor);
                open.push_back(successor);
            }
        }

        return true;
    }

    // Insert all flaws of all operators
    flaw_list.insert(flaw_list.end(), local_flaws.begin(), local_flaws.end());

    return false;
}

template class BFSFlawFinder<MaxProbProjection>;
template class BFSFlawFinder<ExpCostProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd