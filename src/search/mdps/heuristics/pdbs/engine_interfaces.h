#pragma once

#include "../../engine_interfaces/action_evaluator.h"
#include "../../engine_interfaces/action_id_map.h"
#include "../../engine_interfaces/applicable_actions_generator.h"
#include "../../engine_interfaces/state_evaluator.h"
#include "../../engine_interfaces/state_reward_function.h"
#include "../../engine_interfaces/state_id_map.h"
#include "../../engine_interfaces/transition_generator.h"
#include "abstract_operator.h"
#include "abstract_state.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>

namespace successor_generator {
template<typename T>
class SuccessorGenerator;
}

namespace pdbs {
class PatternDatabase;
}

namespace probabilistic {

template<>
class StateIDMap<pdbs::AbstractState> {
public:
    using seen_iterator = std::set<int>::const_iterator;

    explicit StateIDMap() = default;

    StateID get_state_id(const pdbs::AbstractState& state);
    pdbs::AbstractState get_state(const StateID& id);

    unsigned size() const;

    seen_iterator seen_begin() const;
    seen_iterator seen_end() const;

private:
    std::set<int> seen;
};

template<>
class ActionIDMap<const pdbs::AbstractOperator*> {
public:
    explicit ActionIDMap(const std::vector<pdbs::AbstractOperator>& ops);

    ActionID get_action_id(
        const StateID& state_id,
        const pdbs::AbstractOperator* action) const;

    const pdbs::AbstractOperator*
    get_action(const StateID& state_id, const ActionID& action_id) const;

private:
    const std::vector<pdbs::AbstractOperator>& ops_;
};

template<>
class ApplicableActionsGenerator<const pdbs::AbstractOperator*> {
public:
    explicit ApplicableActionsGenerator(
        StateIDMap<pdbs::AbstractState>& id_map_,
        std::shared_ptr<pdbs::AbstractStateMapper> state_mapper,
        std::shared_ptr<successor_generator::SuccessorGenerator<
            const pdbs::AbstractOperator*>> aops_gen);

    void operator()(
        const StateID& state,
        std::vector<const pdbs::AbstractOperator*>& aops);

private:
    StateIDMap<pdbs::AbstractState>& id_map_;
    std::shared_ptr<pdbs::AbstractStateMapper> state_mapper_;
    std::vector<int> values_;
    std::shared_ptr<
        successor_generator::SuccessorGenerator<const pdbs::AbstractOperator*>>
        aops_gen_;
};

template<>
class TransitionGenerator<const pdbs::AbstractOperator*> {
public:
    explicit TransitionGenerator(
        StateIDMap<pdbs::AbstractState>& id_map,
        std::shared_ptr<pdbs::AbstractStateMapper> state_mapper,
        std::shared_ptr<successor_generator::SuccessorGenerator<
            const pdbs::AbstractOperator*>> aops_gen);

    void operator()(
        const StateID& state,
        const pdbs::AbstractOperator* op,
        Distribution<StateID>& result);

    void operator()(
        const StateID& state,
        std::vector<const pdbs::AbstractOperator*>& aops,
        std::vector<Distribution<StateID>>& result);

private:
    StateIDMap<pdbs::AbstractState>& id_map_;
    std::shared_ptr<pdbs::AbstractStateMapper> state_mapper_;
    std::vector<int> values_;
    std::shared_ptr<
        successor_generator::SuccessorGenerator<const pdbs::AbstractOperator*>>
        aops_gen_;
};

namespace pdbs {

using AbstractStateRewardFunction = StateRewardFunction<AbstractState>;
using AbstractStateEvaluator = StateEvaluator<AbstractState>;
using AbstractOperatorRewardFunction = ActionRewardFunction<const AbstractOperator*>;

class QualitativeResultStore;
class ExpCostProjection;

class AbstractStateDeadendStoreEvaluator : public AbstractStateEvaluator {
public:
    explicit AbstractStateDeadendStoreEvaluator(
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

class AbstractStateInStoreRewardFunction : public AbstractStateRewardFunction {
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

class AbstractStateInSetRewardFunction : public AbstractStateRewardFunction {
public:
    explicit AbstractStateInSetRewardFunction(
        const std::unordered_set<AbstractState>* states_,
        value_type::value_t value_in,
        value_type::value_t value_not_in);

protected:
    EvaluationResult evaluate(const AbstractState& state) override;

private:
    const std::unordered_set<AbstractState>* states_;
    const value_type::value_t value_in_;
    const value_type::value_t value_not_in_;
};

class PDBEvaluator : public AbstractStateEvaluator {
public:
    explicit PDBEvaluator(const ::pdbs::PatternDatabase& pdb);

protected:
    EvaluationResult evaluate(const AbstractState& state) override;

private:
    const ::pdbs::PatternDatabase& pdb;
};

class ExpCostPDBEvaluator : public AbstractStateEvaluator {
    const ExpCostProjection& pdb;
    const AbstractStateMapper* mapper;
    int left_multiplier;
    int right_multiplier;
    int domain_size;

public:
    explicit ExpCostPDBEvaluator(
        const ExpCostProjection& pdb,
        const AbstractStateMapper* mapper,
        int add_var);

protected:
    EvaluationResult evaluate(const AbstractState& state) override;

private:
    AbstractState to_parent_state(AbstractState state);
};

class ZeroCostActionEvaluator : public AbstractOperatorRewardFunction {
protected:
    value_type::value_t
    evaluate(StateID, const AbstractOperator*) override;
};

class UnitCostActionEvaluator : public AbstractOperatorRewardFunction {
protected:
    value_type::value_t
    evaluate(StateID, const AbstractOperator*) override;
};

class NormalCostActionEvaluator : public AbstractOperatorRewardFunction {
protected:
    value_type::value_t
    evaluate(StateID, const AbstractOperator*) override;
};

} // namespace pdbs
} // namespace probabilistic
