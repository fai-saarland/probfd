#ifndef PROBFD_HEURISTICS_PDBS_ENGINE_INTERFACES_H
#define PROBFD_HEURISTICS_PDBS_ENGINE_INTERFACES_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/match_tree.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"

#include "probfd/engine_interfaces/action_id_map.h"
#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/state_evaluator.h"
#include "probfd/engine_interfaces/state_id_map.h"
#include "probfd/engine_interfaces/transition_generator.h"

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
namespace engine_interfaces {
template <>
class StateIDMap<heuristics::pdbs::StateRank> {
public:
    StateID get_state_id(heuristics::pdbs::StateRank state) const
    {
        return state.id;
    }

    heuristics::pdbs::StateRank get_state(StateID id) const
    {
        return heuristics::pdbs::StateRank(id);
    }
};

template <>
class ActionIDMap<const heuristics::pdbs::AbstractOperator*> {
    const heuristics::pdbs::MatchTree& aops_gen_;

public:
    explicit ActionIDMap(const heuristics::pdbs::MatchTree& aops_gen)
        : aops_gen_(aops_gen)
    {
    }

    ActionID
    get_action_id(StateID, const heuristics::pdbs::AbstractOperator* op) const
    {
        return aops_gen_.get_operator_index(*op);
    }

    const heuristics::pdbs::AbstractOperator*
    get_action(StateID, ActionID action_id) const
    {
        return &aops_gen_.get_index_operator(action_id.id);
    }
};

template <>
class TransitionGenerator<const heuristics::pdbs::AbstractOperator*> {
    const heuristics::pdbs::MatchTree& aops_gen_;

public:
    explicit TransitionGenerator(const heuristics::pdbs::MatchTree& aops_gen);

    void generate_applicable_actions(
        StateID state,
        std::vector<const heuristics::pdbs::AbstractOperator*>& aops);

    void generate_action_transitions(
        StateID state,
        const heuristics::pdbs::AbstractOperator* op,
        Distribution<StateID>& result);

    void generate_all_transitions(
        StateID state,
        std::vector<const heuristics::pdbs::AbstractOperator*>& aops,
        std::vector<Distribution<StateID>>& result);
};

} // namespace engine_interfaces

namespace heuristics {
namespace pdbs {

using StateRankEvaluator = engine_interfaces::StateEvaluator<StateRank>;
using AbstractCostFunction =
    engine_interfaces::CostFunction<StateRank, const AbstractOperator*>;

class QualitativeResultStore;

class SSPPatternDatabase;
class MaxProbPatternDatabase;

class PDBEvaluator : public StateRankEvaluator {
public:
    explicit PDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    EvaluationResult evaluate(const StateRank& state) const override;

private:
    const ::pdbs::PatternDatabase& pdb;
};

class DeadendPDBEvaluator : public StateRankEvaluator {
public:
    explicit DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb);

    EvaluationResult evaluate(const StateRank& state) const override;

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

    EvaluationResult evaluate(const StateRank& state) const override;
};

template <typename PDBType>
class MergeEvaluator : public engine_interfaces::StateEvaluator<StateRank> {
    const StateRankingFunction& mapper;
    const PDBType& left;
    const PDBType& right;

public:
    MergeEvaluator(
        const StateRankingFunction& mapper,
        const PDBType& left,
        const PDBType& right);

protected:
    EvaluationResult evaluate(const StateRank& state) const override;
};

class BaseAbstractCostFunction : public AbstractCostFunction {
protected:
    const std::vector<bool>& goal_state_flags_;
    const value_t value_in_;
    const value_t value_not_in_;

public:
    explicit BaseAbstractCostFunction(
        const std::vector<bool>& goal_state_flags,
        value_t value_in,
        value_t value_not_in)
        : goal_state_flags_(goal_state_flags)
        , value_in_(value_in)
        , value_not_in_(value_not_in)
    {
    }

    TerminationInfo get_termination_info(const StateRank& state) override
    {
        const bool is_contained = goal_state_flags_[state.id];
        return TerminationInfo(
            is_contained,
            is_contained ? value_in_ : value_not_in_);
    }
};

class ZeroCostAbstractCostFunction : public BaseAbstractCostFunction {
public:
    using BaseAbstractCostFunction::BaseAbstractCostFunction;

    value_t get_action_cost(StateID, const AbstractOperator*) override
    {
        return 0;
    }
};

class NormalCostAbstractCostFunction : public BaseAbstractCostFunction {
public:
    using BaseAbstractCostFunction::BaseAbstractCostFunction;

    value_t get_action_cost(StateID, const AbstractOperator* op) override
    {
        return op->cost;
    }
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__