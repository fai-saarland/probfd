#ifndef PROBFD_HEURISTICS_PDBS_ENGINE_INTERFACES_H
#define PROBFD_HEURISTICS_PDBS_ENGINE_INTERFACES_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/match_tree.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"

#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/evaluator.h"
#include "probfd/engine_interfaces/state_space.h"

#include "utils/collections.h"

#include <memory>
#include <ranges>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace pdbs {
class PatternDatabase;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

using StateRankEvaluator = engine_interfaces::Evaluator<StateRank>;
using AbstractCostFunction =
    engine_interfaces::SimpleCostFunction<StateRank, const AbstractOperator*>;

class ProjectionStateSpace;

class PDBEvaluator : public StateRankEvaluator {
public:
    explicit PDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    EvaluationResult evaluate(StateRank state) const override;

private:
    const ::pdbs::PatternDatabase& pdb;
};

class DeadendPDBEvaluator : public StateRankEvaluator {
public:
    explicit DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    EvaluationResult evaluate(StateRank state) const override;

private:
    const ::pdbs::PatternDatabase& pdb;
};

class IncrementalPPDBEvaluatorBase : public StateRankEvaluator {
    int left_multiplier;
    int right_multiplier;
    int domain_size;

public:
    explicit IncrementalPPDBEvaluatorBase(
        const StateRankingFunction* mapper,
        int add_var);

protected:
    StateRank to_parent_state(StateRank state) const;
};

template <typename PDBType>
class IncrementalPPDBEvaluator : public IncrementalPPDBEvaluatorBase {
    const PDBType& pdb;

public:
    explicit IncrementalPPDBEvaluator(
        const PDBType& pdb,
        const StateRankingFunction* mapper,
        int add_var);

    EvaluationResult evaluate(StateRank state) const override;
};

template <typename PDBType>
class MergeEvaluator : public engine_interfaces::Evaluator<StateRank> {
    const StateRankingFunction& mapper;
    const PDBType& left;
    const PDBType& right;

public:
    MergeEvaluator(
        const StateRankingFunction& mapper,
        const PDBType& left,
        const PDBType& right);

protected:
    EvaluationResult evaluate(StateRank state) const override;
};

class ProjectionCostFunction : public AbstractCostFunction {
    TaskCostFunction* parent_cost_function;
    std::vector<bool> goal_state_flags_;

public:
    ProjectionCostFunction(
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

        auto goals =
            ranking_function.state_ranks(base, std::move(non_goal_vars));

        for (const auto& g : goals) {
            goal_state_flags_[g.id] = true;
        }
    }

    bool is_goal(StateRank state) const override
    {
        return goal_state_flags_[state.id];
    }

    value_t get_action_cost(const AbstractOperator* op) override
    {
        return parent_cost_function->get_action_cost(
            OperatorID(op->original_operator_id));
    }
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__