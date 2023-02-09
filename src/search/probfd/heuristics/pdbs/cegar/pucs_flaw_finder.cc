#include "probfd/heuristics/pdbs/cegar/pucs_flaw_finder.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/pattern_collection_generator_cegar.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"
#include "probfd/heuristics/pdbs/state_rank.h"


#include "task_utils/task_properties.h"

#include "utils/collections.h"

#include "option_parser.h"
#include "plugin.h"

using namespace std;
using namespace utils;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

template <typename PDBType>
PUCSFlawFinder<PDBType>::PUCSFlawFinder(
    const ProbabilisticTask* task,
    unsigned int violation_threshold)
    : FlawFindingStrategy<PDBType>(task)
    , probabilities(
          16,
          typename FlawFindingStrategy<PDBType>::StateHash(this->task_proxy))
    , violation_threshold(violation_threshold)
{
}

template <typename PDBType>
bool PUCSFlawFinder<PDBType>::apply_policy(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    std::vector<int>& init,
    std::vector<Flaw>& flaw_list)
{
    assert(pq.empty() && probabilities.empty());

    // Push initial state for expansion
    pq.push(1.0, init);
    probabilities[init] = 1.0;

    unsigned int violations = 0;
    bool executable = true;

    while (!pq.empty()) {
        auto [priority, current] = pq.pop();

        // TODO remove this once we have a real priority queue...
        if (priority < probabilities[current]) {
            continue;
        }

        bool successful =
            expand(base, solution_index, current, priority, flaw_list);

        executable = executable && successful;

        // Check if a flaw occured.
        if (!successful && ++violations >= violation_threshold) {
            break;
        }
    }

    pq.clear();
    probabilities.clear();
    return executable;
}

template <typename PDBType>
bool PUCSFlawFinder<PDBType>::expand(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    std::vector<int>& state,
    value_t priority,
    std::vector<Flaw>& flaw_list)
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
        assert(solution.is_goal(abs));

        State terminal_state(*this->task, std::move(state));

        if (solution.is_goal(abs) &&
            !task_properties::is_goal_state(this->task_proxy, terminal_state)) {
            if (!base.ignore_goal_violations) {
                // Collect all non-satisfied goal variables that are still
                // available.
                for (FactProxy fact : this->task_proxy.get_goals()) {
                    const auto& [goal_var, goal_val] = fact.get_pair();

                    if (terminal_state[goal_var].get_value() != goal_val &&
                        !base.global_blacklist.contains(goal_var) &&
                        utils::contains(base.remaining_goals, goal_var)) {
                        flaw_list.emplace_back(true, solution_index, goal_var);
                    }
                }
            }

            return false;
        }

        return true;
    }

    std::vector<Flaw> local_flaws;

    const ProbabilisticOperatorsProxy operators =
        this->task_proxy.get_operators();

    for (const AbstractOperator* abs_op : abs_operators) {
        int original_id = abs_op->original_operator_id;
        const ProbabilisticOperatorProxy& op = operators[original_id];

        // Check whether precondition flaws occur
        bool preconditions_ok = true;

        for (const FactProxy precondition : op.get_preconditions()) {
            const auto& [pre_var, pre_val] = precondition.get_pair();
            // We ignore blacklisted variables
            const bool is_blacklist_var =
                base.global_blacklist.contains(pre_var);

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

        // Generate the successors and add them to the queue
        for (const ProbabilisticOutcomeProxy outcome : op.get_outcomes()) {
            const auto succ_prob = priority * outcome.get_probability();
            auto successor = this->apply_op_to_state(state, outcome);

            auto [it, inserted] = probabilities.emplace(successor, succ_prob);

            if (!inserted) {
                if (succ_prob <= it->second) {
                    continue;
                }

                it->second = succ_prob;
            }

            pq.push(succ_prob, std::move(successor));
        }

        return true;
    }

    // Insert all flaws of all operators
    flaw_list.insert(flaw_list.end(), local_flaws.begin(), local_flaws.end());

    return false;
}

template class PUCSFlawFinder<MaxProbPatternDatabase>;
template class PUCSFlawFinder<SSPPatternDatabase>;

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd