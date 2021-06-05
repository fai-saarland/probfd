#pragma once

#include "../../engine_interfaces/action_evaluator.h"
#include "../../engine_interfaces/action_id_map.h"
#include "../../engine_interfaces/applicable_actions_generator.h"
#include "../../engine_interfaces/state_evaluator.h"
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
class StateEvaluator<pdbs::AbstractState> {
public:
    virtual ~StateEvaluator() = default;
    EvaluationResult operator()(const pdbs::AbstractState& state)
    {
        return evaluate(state);
    }

protected:
    virtual EvaluationResult evaluate(const pdbs::AbstractState& state) = 0;
};

template<>
class ActionEvaluator<const pdbs::AbstractOperator*> {
public:
    virtual ~ActionEvaluator() = default;
    value_type::value_t
    operator()(const StateID&, const pdbs::AbstractOperator* op)
    {
        return this->evaluate(op);
    }

protected:
    virtual value_type::value_t evaluate(const pdbs::AbstractOperator* op) = 0;
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

class QualitativeResultStore;

class AbstractStateInStoreEvaluator : public StateEvaluator<AbstractState> {
public:
    explicit AbstractStateInStoreEvaluator(
        const QualitativeResultStore* states_,
        value_type::value_t value_in,
        value_type::value_t value_not_in);

protected:
    virtual EvaluationResult evaluate(const AbstractState& state) override;

private:
    const QualitativeResultStore* states_;
    const value_type::value_t value_in_;
    const value_type::value_t value_not_in_;
};

class AbstractStateInSetEvaluator : public StateEvaluator<AbstractState> {
public:
    explicit AbstractStateInSetEvaluator(
        const std::unordered_set<AbstractState>* states_,
        value_type::value_t value_in,
        value_type::value_t value_not_in);

protected:
    virtual EvaluationResult evaluate(const AbstractState& state) override;

private:
    const std::unordered_set<AbstractState>* states_;
    const value_type::value_t value_in_;
    const value_type::value_t value_not_in_;
};

class PDBEvaluator : public StateEvaluator<AbstractState> {
public:
    explicit PDBEvaluator(const ::pdbs::PatternDatabase& pdb);

protected:
    virtual EvaluationResult evaluate(const AbstractState& state) override;

private:
    const ::pdbs::PatternDatabase& pdb;
};

class ZeroCostActionEvaluator
    : public ActionEvaluator<const AbstractOperator*> {
protected:
    virtual value_type::value_t evaluate(const AbstractOperator*) override;
};

class UnitCostActionEvaluator
    : public ActionEvaluator<const AbstractOperator*> {
protected:
    virtual value_type::value_t evaluate(const AbstractOperator*) override;
};

class NormalCostActionEvaluator
    : public ActionEvaluator<const AbstractOperator*> {
protected:
    virtual value_type::value_t evaluate(const AbstractOperator*) override;
};

using AbstractStateEvaluator = StateEvaluator<AbstractState>;

using AbstractOperatorEvaluator = ActionEvaluator<const AbstractOperator*>;

} // namespace pdbs
} // namespace probabilistic
