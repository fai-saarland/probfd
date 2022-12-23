#ifndef MDPS_HEURISTICS_PDBS_ENGINE_INTERFACES_H
#define MDPS_HEURISTICS_PDBS_ENGINE_INTERFACES_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/abstract_state_mapper.h"
#include "probfd/heuristics/pdbs/match_tree.h"

#include "probfd/engine_interfaces/action_id_map.h"
#include "probfd/engine_interfaces/reward_function.h"
#include "probfd/engine_interfaces/state_evaluator.h"
#include "probfd/engine_interfaces/state_id_map.h"
#include "probfd/engine_interfaces/transition_generator.h"

#include "utils/collections.h"
#include "utils/range_proxy.h"

#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace successor_generator {
template <typename T>
class SuccessorGenerator;
}

namespace pdbs {
class PatternDatabase;
}

namespace probfd {
namespace engine_interfaces {
template <>
class StateIDMap<heuristics::pdbs::AbstractState> {
public:
    using visited_iterator = std::set<int>::const_iterator;
    using visited_range = utils::RangeProxy<visited_iterator>;

    explicit StateIDMap() = default;

    StateID get_state_id(const heuristics::pdbs::AbstractState& state);
    heuristics::pdbs::AbstractState get_state(const StateID& id);

    unsigned size() const;

    visited_iterator visited_begin() const;
    visited_iterator visited_end() const;

    visited_range visited() const;

private:
    std::set<int> seen;
};

template <>
class ActionIDMap<const heuristics::pdbs::AbstractOperator*> {
public:
    explicit ActionIDMap(
        const std::vector<heuristics::pdbs::AbstractOperator>& ops);

    ActionID get_action_id(
        const StateID& state_id,
        const heuristics::pdbs::AbstractOperator* action) const;

    const heuristics::pdbs::AbstractOperator*
    get_action(const StateID& state_id, const ActionID& action_id) const;

private:
    const std::vector<heuristics::pdbs::AbstractOperator>& ops_;
};

template <>
class TransitionGenerator<const heuristics::pdbs::AbstractOperator*> {
public:
    explicit TransitionGenerator(
        StateIDMap<heuristics::pdbs::AbstractState>& id_map,
        const heuristics::pdbs::MatchTree& aops_gen);

    void operator()(
        const StateID& state,
        std::vector<const heuristics::pdbs::AbstractOperator*>& aops);

    void operator()(
        const StateID& state,
        const heuristics::pdbs::AbstractOperator* op,
        Distribution<StateID>& result);

    void operator()(
        const StateID& state,
        std::vector<const heuristics::pdbs::AbstractOperator*>& aops,
        std::vector<Distribution<StateID>>& result);

private:
    StateIDMap<heuristics::pdbs::AbstractState>& id_map_;
    const heuristics::pdbs::MatchTree& aops_gen_;
};

} // namespace engine_interfaces

namespace heuristics {
namespace pdbs {

using AbstractStateEvaluator = engine_interfaces::StateEvaluator<AbstractState>;
using AbstractRewardFunction =
    engine_interfaces::RewardFunction<AbstractState, const AbstractOperator*>;

class QualitativeResultStore;

class ExpCostProjection;
class MaxProbProjection;

class PDBEvaluator : public AbstractStateEvaluator {
public:
    explicit PDBEvaluator(const ::pdbs::PatternDatabase& pdb);

protected:
    EvaluationResult evaluate(const AbstractState& state) const override;

private:
    const ::pdbs::PatternDatabase& pdb;
};

class DeadendPDBEvaluator : public AbstractStateEvaluator {
public:
    explicit DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb);

protected:
    EvaluationResult evaluate(const AbstractState& state) const override;

private:
    const ::pdbs::PatternDatabase& pdb;
};

class IncrementalPPDBEvaluatorBase : public AbstractStateEvaluator {
    int left_multiplier;
    int right_multiplier;
    int domain_size;

public:
    explicit IncrementalPPDBEvaluatorBase(
        const AbstractStateMapper* mapper,
        int add_var);

protected:
    AbstractState to_parent_state(AbstractState state) const;
};

template <typename PDBType>
class IncrementalPPDBEvaluator : public IncrementalPPDBEvaluatorBase {
    const PDBType& pdb;

public:
    explicit IncrementalPPDBEvaluator(
        const PDBType& pdb,
        const AbstractStateMapper* mapper,
        int add_var);

protected:
    EvaluationResult evaluate(const AbstractState& state) const override;
};

class BaseAbstractRewardFunction : public AbstractRewardFunction {
protected:
    const std::vector<bool>& goal_state_flags_;
    const value_type::value_t value_in_;
    const value_type::value_t value_not_in_;

public:
    explicit BaseAbstractRewardFunction(
        const std::vector<bool>& goal_state_flags,
        value_type::value_t value_in,
        value_type::value_t value_not_in)
        : goal_state_flags_(goal_state_flags)
        , value_in_(value_in)
        , value_not_in_(value_not_in)
    {
    }

protected:
    TerminationInfo evaluate(const AbstractState& state) override
    {
        const bool is_contained = goal_state_flags_[state.id];
        return TerminationInfo(
            is_contained,
            is_contained ? value_in_ : value_not_in_);
    }
};

class ZeroCostAbstractRewardFunction : public BaseAbstractRewardFunction {
public:
    using BaseAbstractRewardFunction::BaseAbstractRewardFunction;

protected:
    value_type::value_t evaluate(StateID, const AbstractOperator*) override
    {
        return 0;
    }
};

class NormalCostAbstractRewardFunction : public BaseAbstractRewardFunction {
public:
    using BaseAbstractRewardFunction::BaseAbstractRewardFunction;

protected:
    value_type::value_t evaluate(StateID, const AbstractOperator* op) override
    {
        return op->reward;
    }
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__