#include "probfd/heuristics/pdbs/cegar/sampling_flaw_finder.h"

#include "probfd/heuristics/pdbs/expcost_projection.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"
#include "probfd/heuristics/pdbs/pattern_collection_generator_cegar.h"
#include "probfd/heuristics/pdbs/state_rank.h"

#include "probfd/distribution.h"

#include "task_utils/task_properties.h"

#include "utils/collections.h"
#include "utils/rng.h"

#include "option_parser.h"
#include "plugin.h"

#include <stack>

using namespace std;
using namespace utils;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

template <typename PDBType>
SamplingFlawFinder<PDBType>::SamplingFlawFinder(
    const ProbabilisticTask* task,
    unsigned int violation_threshold)
    : FlawFindingStrategy<PDBType>(task)
    , einfos(
          16,
          typename FlawFindingStrategy<PDBType>::StateHash(this->task_proxy))
    , violation_threshold(violation_threshold)
{
}

template <typename PDBType>
bool SamplingFlawFinder<PDBType>::apply_policy(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    std::vector<int>& init,
    std::vector<Flaw>& flaw_list)
{
    assert(stk.empty() && einfos.empty());

    bool executable = true;
    unsigned int violation = 0;

    {
        int status = push_state(base, solution_index, init, flaw_list);
        if ((status & STATE_PUSHED) == 0) {
            assert(stk.empty() && einfos.empty());
            return (status & FLAW_OCCURED) == 0;
        }
    }

    assert(!stk.empty());

    do {
        const std::vector<int>& current = stk.top();
        ExplorationInfo& einfo = einfos[current];

        while (!einfo.successors.empty()) {
            // Sample next successor
            auto it = einfo.successors.sample(*base.rng);
            auto& succ = it->item;

            // Ignore states already seen
            if (einfos.find(succ) == einfos.end()) {
                unsigned int status =
                    push_state(base, solution_index, succ, flaw_list);

                // Recurse if the state was pushed
                if (status & STATE_PUSHED) {
                    einfo.successors.erase(it);
                    goto continue_exploration;
                }

                // Otherwise, check if the state was not pushed due to a flaw
                if (status & FLAW_OCCURED) {
                    executable = false;

                    if (++violation >= violation_threshold) {
                        goto break_exploration;
                    }
                }
            }

            // Don't sample it again
            einfo.successors.erase(it);
        }

        stk.pop();

    continue_exploration:;
    } while (!stk.empty());

break_exploration:;

    std::stack<std::vector<int>>().swap(stk); // Clear stack
    einfos.clear();
    return executable;
}

template <typename PDBType>
unsigned int SamplingFlawFinder<PDBType>::push_state(
    PatternCollectionGeneratorCegar<PDBType>& base,
    int solution_index,
    std::vector<int>& state,
    std::vector<Flaw>& flaw_list)
{
    AbstractSolutionData<PDBType>& solution = *base.solutions[solution_index];
    const AbstractPolicy& policy = solution.get_policy();
    const PDBType& pdb = solution.get_pdb();

    // Check flaws, generate successors
    const StateRank abs = pdb.get_abstract_state(state);

    // We reached a dead-end, the operator is irrelevant.
    if (pdb.is_dead_end(abs)) {
        return 0;
    }

    const auto abs_operators = policy[abs];

    // We reached a terminal state, check if it is a goal
    if (abs_operators.empty()) {
        assert(pdb.is_goal(abs));

        State terminal_state(*this->task, move(state));

        if (pdb.is_goal(abs) &&
            !task_properties::is_goal_state(this->task_proxy, terminal_state)) {
            if (!base.ignore_goal_violations) {
                // Collect all non-satisfied goal variables that are
                // still available.
                for (FactProxy fact : this->task_proxy.get_goals()) {
                    const auto& [goal_var, goal_val] = fact.get_pair();

                    if (state[goal_var] != goal_val &&
                        !utils::contains(base.global_blacklist, goal_var) &&
                        utils::contains(base.remaining_goals, goal_var)) {
                        flaw_list.emplace_back(true, solution_index, goal_var);
                    }
                }
            }

            return FLAW_OCCURED;
        }

        return 0;
    }

    std::vector<Flaw> local_flaws;

    const ProbabilisticOperatorsProxy operators =
        this->task_proxy.get_operators();

    for (const AbstractOperator* abs_op : abs_operators) {
        int original_id = abs_op->original_operator_id;
        const ProbabilisticOperatorProxy op = operators[original_id];

        // Check whether precondition flaws occur
        bool preconditions_ok = true;

        for (const FactProxy precondition : op.get_preconditions()) {
            const auto& [pre_var, pre_val] = precondition.get_pair();

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

        // Generate the successors
        assert(einfos.find(state) == einfos.end());
        ExplorationInfo& einfo = einfos[state];
        for (const ProbabilisticOutcomeProxy outcome : op.get_outcomes()) {
            einfo.successors.add_unique(
                this->apply_op_to_state(state, outcome),
                outcome.get_probability());
        }

        stk.push(state);
        return STATE_PUSHED;
    }

    // Insert all flaws of all operators
    flaw_list.insert(flaw_list.end(), local_flaws.begin(), local_flaws.end());

    return FLAW_OCCURED;
}

template class SamplingFlawFinder<MaxProbProjection>;
template class SamplingFlawFinder<ExpCostProjection>;

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd