#pragma once

#include "../../algorithms/types_common.h"
#include "../../distribution.h"
#include "../../evaluation_result.h"
#include "../../value_type.h"
#include "abstract_state.h"

#include <memory>
#include <vector>
#include <string>

namespace successor_generator {
template<typename T>
class SuccessorGenerator;
}

namespace probabilistic {

class ProbabilisticOperator;

namespace pdbs {

class QualitativeResultStore;

class AbstractOperator {
public:
    explicit AbstractOperator(unsigned id);
    AbstractOperator(AbstractOperator&&) = default;
    unsigned original_operator_id;
    AbstractState pre;
    Distribution<AbstractState> outcomes;
};

class AbstractOperatorToString {
public:
    explicit AbstractOperatorToString(
        const std::vector<ProbabilisticOperator>* ops);
    std::string operator()(const AbstractOperator* op);

private:
    const std::vector<ProbabilisticOperator>* ops_;
};

} // namespace pdbs

namespace algorithms {

template<>
class ApplicableActionsGenerator<
    pdbs::AbstractState,
    const pdbs::AbstractOperator*> {
public:
    explicit ApplicableActionsGenerator(
        std::shared_ptr<pdbs::AbstractStateMapper> state_mapper,
        std::shared_ptr<successor_generator::SuccessorGenerator<
            const pdbs::AbstractOperator*>> aops_gen);
    void operator()(
        const pdbs::AbstractState& state,
        std::vector<const pdbs::AbstractOperator*>& aops);

private:
    std::shared_ptr<pdbs::AbstractStateMapper> state_mapper_;
    std::vector<int> values_;
    std::shared_ptr<
        successor_generator::SuccessorGenerator<const pdbs::AbstractOperator*>>
        aops_gen_;
};

template<>
class TransitionGenerator<pdbs::AbstractState, const pdbs::AbstractOperator*> {
public:
    Distribution<pdbs::AbstractState> operator()(
        const pdbs::AbstractState& state,
        const pdbs::AbstractOperator* op) const;
};

template<>
class StateIDMap<pdbs::AbstractState> {
public:
    using indirection_iterator = std::unordered_map<int, int>::const_iterator;

    explicit StateIDMap();

    StateID operator[](const pdbs::AbstractState& state);
    pdbs::AbstractState operator[](const StateID& id);

    indirection_iterator indirection_begin() const;
    indirection_iterator indirection_end() const;

    unsigned size() const;

private:
    int next_id_;
    std::unordered_map<int, int> id_map_;
    std::vector<int> inverse_map_;
};

template<>
class StateEvaluationFunction<pdbs::AbstractState> {
public:
    virtual ~StateEvaluationFunction() = default;
    EvaluationResult operator()(const pdbs::AbstractState& state)
    {
        return evaluate(state);
    }

protected:
    virtual EvaluationResult evaluate(const pdbs::AbstractState& state) = 0;
};

template<>
class TransitionRewardFunction<
    pdbs::AbstractState,
    const pdbs::AbstractOperator*> {
public:
    virtual ~TransitionRewardFunction() = default;
    value_type::value_t
    operator()(const pdbs::AbstractState& s, const pdbs::AbstractOperator* op)
    {
        return this->evaluate(s, op);
    }

protected:
    virtual value_type::value_t evaluate(
        const pdbs::AbstractState& s,
        const pdbs::AbstractOperator* op) = 0;
};
} // namespace algorithms

namespace pdbs {
class AbstractStateInStore
    : public algorithms::StateEvaluationFunction<AbstractState> {
public:
    explicit AbstractStateInStore(
        QualitativeResultStore* states_,
        value_type::value_t value_in,
        value_type::value_t value_not_in);

protected:
    virtual EvaluationResult evaluate(const AbstractState& state) override;

private:
    QualitativeResultStore* states_;
    const value_type::value_t value_in_;
    const value_type::value_t value_not_in_;
};

class AbstractTransitionNoReward
    : public algorithms::
          TransitionRewardFunction<AbstractState, const AbstractOperator*> {
protected:
    virtual value_type::value_t
    evaluate(const AbstractState&, const AbstractOperator*) override;
};

using AbstractStateEvaluator =
    algorithms::StateEvaluationFunction<AbstractState>;

using AbstractTransitionRewardFunction = algorithms::
    TransitionRewardFunction<AbstractState, const AbstractOperator*>;

} // namespace pdbs
} // namespace probabilistic
