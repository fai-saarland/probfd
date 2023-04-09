#include "probfd/heuristics/pdbs/engine_interfaces.h"

#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"

#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include "pdbs/pattern_database.h"

#include "utils/collections.h"

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
    const ProbabilisticPatternDatabase& pdb,
    const StateRankingFunction* mapper,
    int add_var)
    : pdb(pdb)
{
    const int idx = mapper->get_index(add_var);
    assert(idx != -1);

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
    const ProbabilisticPatternDatabase& left,
    const ProbabilisticPatternDatabase& right)
    : mapper(mapper)
    , left(left)
    , right(right)
{
}

EvaluationResult MergeEvaluator::evaluate(StateRank state) const
{
    StateRank lstate = mapper.convert(state, left.get_pattern());

    auto leval = left.evaluate(lstate);

    if (leval.is_unsolvable()) {
        return leval;
    }

    StateRank rstate = mapper.convert(state, right.get_pattern());

    auto reval = right.evaluate(rstate);

    if (reval.is_unsolvable()) {
        return reval;
    }

    return {false, std::max(leval.get_estimate(), reval.get_estimate())};
}

ProjectionCostFunction::ProjectionCostFunction(
    const ProbabilisticTaskProxy& task_proxy,
    const StateRankingFunction& ranking_function,
    TaskCostFunction* parent_cost_function)
    : AbstractCostFunction(
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
                base.id += ranking_function.get_multiplier(v++) * g_val;
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

    auto goals = ranking_function.state_ranks(base, std::move(non_goal_vars));

    for (const auto& g : goals) {
        goal_state_flags_[g.id] = true;
    }
}

bool ProjectionCostFunction::is_goal(StateRank state) const
{
    return goal_state_flags_[state.id];
}

value_t ProjectionCostFunction::get_action_cost(const AbstractOperator* op)
{
    return parent_cost_function->get_action_cost(
        OperatorID(op->original_operator_id));
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
