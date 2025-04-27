#include "probfd/probabilistic_operator_space.h"

#include "probfd/task_utils/task_properties.h"

#include "downward/fact_pair.h"

#include <algorithm>
#include <cassert>

using namespace downward;

namespace probfd {

ProbabilisticEffectConditionsProxy::ProbabilisticEffectConditionsProxy(
    const ProbabilisticOperatorSpace& op_space,
    int op_index,
    int outcome_index,
    int eff_index)
    : op_space_(&op_space)
    , op_index_(op_index)
    , outcome_index_(outcome_index)
    , eff_index_(eff_index)
{
}

std::size_t ProbabilisticEffectConditionsProxy::size() const
{
    return op_space_->get_num_operator_outcome_effect_conditions(
        op_index_,
        outcome_index_,
        eff_index_);
}

FactPair ProbabilisticEffectConditionsProxy::operator[](std::size_t index) const
{
    assert(index < size());
    return op_space_->get_operator_outcome_effect_condition(
        op_index_,
        outcome_index_,
        eff_index_,
        static_cast<int>(index));
}

ProbabilisticEffectProxy::ProbabilisticEffectProxy(
    const ProbabilisticOperatorSpace& op_space,
    int op_index,
    int outcome_index,
    int eff_index)
    : op_space_(&op_space)
    , op_index_(op_index)
    , outcome_index_(outcome_index)
    , eff_index_(eff_index)
{
}

ProbabilisticEffectConditionsProxy
ProbabilisticEffectProxy::get_conditions() const
{
    return ProbabilisticEffectConditionsProxy(
        *op_space_,
        op_index_,
        outcome_index_,
        eff_index_);
}

FactPair ProbabilisticEffectProxy::get_fact() const
{
    return op_space_->get_operator_outcome_effect(
        op_index_,
        outcome_index_,
        eff_index_);
}

ProbabilisticEffectsProxy::ProbabilisticEffectsProxy(
    const ProbabilisticOperatorSpace& op_space,
    int op_index,
    int outcome_index)
    : op_space_(&op_space)
    , op_index_(op_index)
    , outcome_index_(outcome_index)
{
}

std::size_t ProbabilisticEffectsProxy::size() const
{
    return op_space_->get_num_operator_outcome_effects(
        op_index_,
        outcome_index_);
}

ProbabilisticEffectProxy
ProbabilisticEffectsProxy::operator[](std::size_t eff_index) const
{
    assert(eff_index < size());
    return ProbabilisticEffectProxy(
        *op_space_,
        op_index_,
        outcome_index_,
        eff_index);
}

ProbabilisticOutcomeProxy::ProbabilisticOutcomeProxy(
    const ProbabilisticOperatorSpace& op_space,
    int op_index,
    int outcome_index)
    : op_space_(&op_space)
    , op_index_(op_index)
    , outcome_index_(outcome_index)
{
}

int ProbabilisticOutcomeProxy::get_determinization_id() const
{
    return op_space_->get_operator_outcome_id(op_index_, outcome_index_);
}

ProbabilisticOperatorProxy ProbabilisticOutcomeProxy::get_operator() const
{
    return ProbabilisticOperatorProxy(*op_space_, op_index_);
}

ProbabilisticEffectsProxy ProbabilisticOutcomeProxy::get_effects() const
{
    return ProbabilisticEffectsProxy(*op_space_, op_index_, outcome_index_);
}

value_t ProbabilisticOutcomeProxy::get_probability() const
{
    return op_space_->get_operator_outcome_probability(
        op_index_,
        outcome_index_);
}

State ProbabilisticOutcomeProxy::get_unregistered_successor(
    const State& state,
    AxiomEvaluator& axiom_evaluator) const
{
    return state.get_unregistered_successor(axiom_evaluator, get_effects());
}

ProbabilisticOutcomesProxy::ProbabilisticOutcomesProxy(
    const ProbabilisticOperatorSpace& op_space,
    int op_index)
    : op_space_(&op_space)
    , op_index_(op_index)
{
}

ProbabilisticOperatorProxy ProbabilisticOutcomesProxy::get_operator() const
{
    return ProbabilisticOperatorProxy(*op_space_, op_index_);
}

std::size_t ProbabilisticOutcomesProxy::size() const
{
    return op_space_->get_num_operator_outcomes(op_index_);
}

ProbabilisticOutcomeProxy
ProbabilisticOutcomesProxy::operator[](std::size_t eff_index) const
{
    assert(eff_index < size());
    return ProbabilisticOutcomeProxy(*op_space_, op_index_, eff_index);
}

ProbabilisticOperatorProxy::ProbabilisticOperatorProxy(
    const ProbabilisticOperatorSpace& op_space,
    int index)
    : op_space_(&op_space)
    , index_(index)
{
}

OperatorPreconditionsProxy ProbabilisticOperatorProxy::get_preconditions() const
{
    return OperatorPreconditionsProxy(*op_space_, index_);
}

ProbabilisticOutcomesProxy ProbabilisticOperatorProxy::get_outcomes() const
{
    return ProbabilisticOutcomesProxy(*op_space_, index_);
}

std::string ProbabilisticOperatorProxy::get_name() const
{
    return op_space_->get_operator_name(index_);
}

int ProbabilisticOperatorProxy::get_id() const
{
    return index_;
}

bool operator==(
    const ProbabilisticOperatorProxy& left,
    const ProbabilisticOperatorProxy& right)
{
    assert(left.op_space_ == right.op_space_);
    return left.index_ == right.index_;
}

} // namespace probfd