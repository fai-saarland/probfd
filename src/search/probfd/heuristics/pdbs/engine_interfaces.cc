#include "probfd/heuristics/pdbs/engine_interfaces.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"

#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include "downward/pdbs/pattern_database.h"

#include "downward/utils/collections.h"

#include <limits>
#include <memory>

namespace probfd {
namespace heuristics {
namespace pdbs {

PDBEvaluator::PDBEvaluator(const ::pdbs::PatternDatabase& pdb)
    : pdb(pdb)
{
}

EvaluationResult PDBEvaluator::evaluate(StateRank state) const
{
    int deterministic_val = pdb.get_value_for_index(state.id);

    if (deterministic_val == std::numeric_limits<int>::max()) {
        return EvaluationResult(true, INFINITE_VALUE);
    }

    return EvaluationResult(false, static_cast<value_t>(deterministic_val));
}

DeadendPDBEvaluator::DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb)
    : pdb(pdb)
{
}

EvaluationResult DeadendPDBEvaluator::evaluate(StateRank state) const
{
    const bool dead =
        pdb.get_value_for_index(state.id) == std::numeric_limits<int>::max();

    return EvaluationResult(dead, dead ? 1_vt : 0_vt);
}

IncrementalPPDBEvaluator::IncrementalPPDBEvaluator(
    const ProbabilityAwarePatternDatabase& pdb,
    const StateRankingFunction* mapper,
    int add_var)
    : pdb(pdb)
{
    const Pattern& pattern = mapper->get_pattern();
    auto it = std::ranges::lower_bound(pattern, add_var);
    assert(it != pattern.end());
    auto idx = std::distance(pattern.begin(), it);

    this->domain_size = mapper->get_domain_size(idx);
    this->left_multiplier = mapper->get_multiplier(idx);
    this->right_multiplier =
        static_cast<unsigned int>(idx + 1) < mapper->num_vars()
            ? mapper->get_multiplier(idx + 1)
            : mapper->num_states();
}

EvaluationResult IncrementalPPDBEvaluator::evaluate(StateRank state) const
{
    return pdb.evaluate(to_parent_state(state));
}

StateRank IncrementalPPDBEvaluator::to_parent_state(StateRank state) const
{
    int left = state.id % left_multiplier;
    int right = state.id - (state.id % right_multiplier);
    return StateRank(left + right / domain_size);
}

MergeEvaluator::MergeEvaluator(
    const StateRankingFunction& mapper,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right)
    : mapper(mapper)
    , left(left)
    , right(right)
{
}

EvaluationResult MergeEvaluator::evaluate(StateRank state) const
{
    const StateRank lstate =
        convert(state, mapper, left.get_state_ranking_function());

    auto leval = left.evaluate(lstate);

    if (leval.is_unsolvable()) {
        return leval;
    }

    const StateRank rstate =
        convert(state, mapper, right.get_state_ranking_function());

    auto reval = right.evaluate(rstate);

    if (reval.is_unsolvable()) {
        return reval;
    }

    return {false, std::max(leval.get_estimate(), reval.get_estimate())};
}

StateRank MergeEvaluator::convert(
    StateRank state_rank,
    const StateRankingFunction& refined_mapping,
    const StateRankingFunction& coarser_mapping) const
{
    const Pattern& larger_pattern = refined_mapping.get_pattern();
    const Pattern& smaller_pattern = coarser_mapping.get_pattern();

    assert(std::ranges::includes(larger_pattern, smaller_pattern));

    StateRank rank(0);

    for (size_t i = 0, j = 0; j != smaller_pattern.size(); ++i, ++j) {
        while (larger_pattern[i] != smaller_pattern[j]) ++i;
        rank.id += coarser_mapping.rank_fact(
            j,
            refined_mapping.value_of(state_rank, i));
    }

    return rank;
}

InducedProjectionCostFunction::InducedProjectionCostFunction(
    const ProbabilisticTaskProxy& task_proxy,
    const StateRankingFunction& ranking_function,
    TaskCostFunction* parent_cost_function)
    : ProjectionCostFunction(
          parent_cost_function->get_goal_termination_cost(),
          parent_cost_function->get_non_goal_termination_cost())
    , parent_cost_function(parent_cost_function)
    , goal_state_flags_(ranking_function.num_states(), false)
{
    const GoalsProxy task_goals = task_proxy.get_goals();
    const Pattern& pattern = ranking_function.get_pattern();

    std::vector<int> non_goal_vars;
    StateRank base(0);

    // Translate sparse goal into pdb index space
    // and collect non-goal variables aswell.
    const int num_goal_facts = task_goals.size();
    const int num_variables = pattern.size();

    for (int v = 0, w = 0; v != static_cast<int>(pattern.size());) {
        const int p_var = pattern[v];
        const FactProxy goal_fact = task_goals[w];
        const int g_var = goal_fact.get_variable().get_id();

        if (p_var < g_var) {
            non_goal_vars.push_back(v++);
        } else {
            if (p_var == g_var) {
                const int g_val = goal_fact.get_value();
                base.id += ranking_function.rank_fact(v++, g_val);
            }

            if (++w == num_goal_facts) {
                while (v < num_variables) {
                    non_goal_vars.push_back(v++);
                }
                break;
            }
        }
    }

    assert(non_goal_vars.size() != pattern.size()); // No goal no fun.

    do {
        goal_state_flags_[base.id] = true;
    } while (ranking_function.next_rank(base, non_goal_vars));
}

bool InducedProjectionCostFunction::is_goal(StateRank state) const
{
    return goal_state_flags_[state.id];
}

value_t
InducedProjectionCostFunction::get_action_cost(const ProjectionOperator* op)
{
    return parent_cost_function->get_action_cost(op->operator_id);
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
