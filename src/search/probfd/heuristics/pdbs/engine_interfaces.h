#ifndef MDPS_HEURISTICS_PDBS_ENGINE_INTERFACES_H
#define MDPS_HEURISTICS_PDBS_ENGINE_INTERFACES_H

#include "../../../utils/collections.h"
#include "../../../utils/range_proxy.h"
#include "../../engine_interfaces/action_id_map.h"
#include "../../engine_interfaces/reward_function.h"
#include "../../engine_interfaces/state_evaluator.h"
#include "../../engine_interfaces/state_id_map.h"
#include "../../engine_interfaces/transition_generator.h"
#include "abstract_operator.h"
#include "abstract_state_mapper.h"

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
        std::shared_ptr<heuristics::pdbs::AbstractStateMapper> state_mapper,
        std::shared_ptr<successor_generator::SuccessorGenerator<
            const heuristics::pdbs::AbstractOperator*>> aops_gen);

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
    std::shared_ptr<heuristics::pdbs::AbstractStateMapper> state_mapper_;
    std::vector<int> values_;
    std::shared_ptr<successor_generator::SuccessorGenerator<
        const heuristics::pdbs::AbstractOperator*>>
        aops_gen_;
};

namespace heuristics {
namespace pdbs {

using AbstractStateEvaluator = StateEvaluator<AbstractState>;
using AbstractRewardFunction =
    RewardFunction<AbstractState, const AbstractOperator*>;

class QualitativeResultStore;

class ExpCostProjection;
class MaxProbProjection;

class AbstractStateDeadendStoreEvaluator : public AbstractStateEvaluator {
public:
    explicit AbstractStateDeadendStoreEvaluator(
        const QualitativeResultStore* states_,
        value_type::value_t value_in,
        value_type::value_t value_not_in);

protected:
    EvaluationResult evaluate(const AbstractState& state) const override;

private:
    const QualitativeResultStore* states_;
    const value_type::value_t value_in_;
    const value_type::value_t value_not_in_;
};

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

class AbstractStateInStoreRewardFunction : public AbstractRewardFunction {
public:
    explicit AbstractStateInStoreRewardFunction(
        const QualitativeResultStore* states_,
        value_type::value_t value_in,
        value_type::value_t value_not_in);

protected:
    EvaluationResult evaluate(const AbstractState& state) override;

private:
    const QualitativeResultStore* states_;
    const value_type::value_t value_in_;
    const value_type::value_t value_not_in_;
};

template <typename Container>
class ZeroCostAbstractRewardFunction : public AbstractRewardFunction {
public:
    explicit ZeroCostAbstractRewardFunction(
        const Container* goal_states,
        value_type::value_t value_in,
        value_type::value_t value_not_in)
        : goal_states_(goal_states)
        , value_in_(value_in)
        , value_not_in_(value_not_in)
    {
    }

protected:
    EvaluationResult evaluate(const AbstractState& state) override
    {
        const bool is_contained = utils::contains(*goal_states_, state);
        return EvaluationResult(
            is_contained,
            is_contained ? value_in_ : value_not_in_);
    }

    value_type::value_t evaluate(StateID, const AbstractOperator*) override
    {
        return 0;
    }

private:
    const Container* goal_states_;
    const value_type::value_t value_in_;
    const value_type::value_t value_not_in_;
};

template <typename Container>
class NormalCostAbstractRewardFunction : public AbstractRewardFunction {
public:
    explicit NormalCostAbstractRewardFunction(
        Container* goal_states,
        value_type::value_t value_in,
        value_type::value_t value_not_in)
        : goal_states_(goal_states)
        , value_in_(value_in)
        , value_not_in_(value_not_in)
    {
    }

protected:
    EvaluationResult evaluate(const AbstractState& state) override
    {
        const bool is_contained = ::utils::contains(*goal_states_, state);
        return EvaluationResult(
            is_contained,
            is_contained ? value_in_ : value_not_in_);
    }

    value_type::value_t evaluate(StateID, const AbstractOperator* op) override
    {
        return op->reward;
    }

private:
    const Container* goal_states_;
    const value_type::value_t value_in_;
    const value_type::value_t value_not_in_;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__