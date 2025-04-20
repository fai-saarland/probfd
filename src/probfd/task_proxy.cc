
#include "probfd/task_proxy.h"

#include "probfd/task_utils/causal_graph.h"

#include "probfd/probabilistic_task.h"
#include "probfd/task_utils/task_properties.h"

#include <algorithm>
#include <cassert>

using namespace downward;

namespace probfd {

ProbabilisticEffectConditionsProxy::ProbabilisticEffectConditionsProxy(
    const ProbabilisticTask& task,
    int op_index,
    int outcome_index,
    int eff_index)
    : task_(&task)
    , op_index_(op_index)
    , outcome_index_(outcome_index)
    , eff_index_(eff_index)
{
}

std::size_t ProbabilisticEffectConditionsProxy::size() const
{
    return task_->get_num_operator_outcome_effect_conditions(
        op_index_,
        outcome_index_,
        eff_index_);
}

FactProxy
ProbabilisticEffectConditionsProxy::operator[](std::size_t index) const
{
    assert(index < size());
    return FactProxy(
        *task_,
        task_->get_operator_outcome_effect_condition(
            op_index_,
            outcome_index_,
            eff_index_,
            static_cast<int>(index)));
}

ProbabilisticEffectProxy::ProbabilisticEffectProxy(
    const ProbabilisticTask& task,
    int op_index,
    int outcome_index,
    int eff_index)
    : task_(&task)
    , op_index_(op_index)
    , outcome_index_(outcome_index)
    , eff_index_(eff_index)
{
}

ProbabilisticEffectConditionsProxy
ProbabilisticEffectProxy::get_conditions() const
{
    return ProbabilisticEffectConditionsProxy(
        *task_,
        op_index_,
        outcome_index_,
        eff_index_);
}

FactProxy ProbabilisticEffectProxy::get_fact() const
{
    return FactProxy(
        *task_,
        task_->get_operator_outcome_effect(
            op_index_,
            outcome_index_,
            eff_index_));
}

ProbabilisticEffectsProxy::ProbabilisticEffectsProxy(
    const ProbabilisticTask& task,
    int op_index,
    int outcome_index)
    : task_(&task)
    , op_index_(op_index)
    , outcome_index_(outcome_index)
{
}

std::size_t ProbabilisticEffectsProxy::size() const
{
    return task_->get_num_operator_outcome_effects(op_index_, outcome_index_);
}

ProbabilisticEffectProxy
ProbabilisticEffectsProxy::operator[](std::size_t eff_index) const
{
    assert(eff_index < size());
    return ProbabilisticEffectProxy(
        *task_,
        op_index_,
        outcome_index_,
        eff_index);
}

ProbabilisticOutcomeProxy::ProbabilisticOutcomeProxy(
    const ProbabilisticTask& task,
    int op_index,
    int outcome_index)
    : task_(&task)
    , op_index_(op_index)
    , outcome_index_(outcome_index)
{
}

int ProbabilisticOutcomeProxy::get_determinization_id() const
{
    return task_->get_operator_outcome_id(op_index_, outcome_index_);
}

ProbabilisticOperatorProxy ProbabilisticOutcomeProxy::get_operator() const
{
    return ProbabilisticOperatorProxy(*task_, op_index_);
}

ProbabilisticEffectsProxy ProbabilisticOutcomeProxy::get_effects() const
{
    return ProbabilisticEffectsProxy(*task_, op_index_, outcome_index_);
}

value_t ProbabilisticOutcomeProxy::get_probability() const
{
    return task_->get_operator_outcome_probability(op_index_, outcome_index_);
}

State ProbabilisticOutcomeProxy::get_unregistered_successor(
    const State& state) const
{
    const PlanningTaskProxy task_proxy(*task_);
    return state.get_unregistered_successor(task_proxy, get_effects());
}

ProbabilisticOutcomesProxy::ProbabilisticOutcomesProxy(
    const ProbabilisticTask& task,
    int op_index)
    : task_(&task)
    , op_index_(op_index)
{
}

ProbabilisticOperatorProxy ProbabilisticOutcomesProxy::get_operator() const
{
    return ProbabilisticOperatorProxy(*task_, op_index_);
}

std::size_t ProbabilisticOutcomesProxy::size() const
{
    return task_->get_num_operator_outcomes(op_index_);
}

ProbabilisticOutcomeProxy
ProbabilisticOutcomesProxy::operator[](std::size_t eff_index) const
{
    assert(eff_index < size());
    return ProbabilisticOutcomeProxy(*task_, op_index_, eff_index);
}

ProbabilisticOperatorProxy::ProbabilisticOperatorProxy(
    const ProbabilisticTask& task,
    int index)
    : PartialOperatorProxy(task, index)
{
}

ProbabilisticOutcomesProxy ProbabilisticOperatorProxy::get_outcomes() const
{
    return ProbabilisticOutcomesProxy(
        *static_cast<const ProbabilisticTask*>(task),
        index);
}

value_t ProbabilisticOperatorProxy::get_cost() const
{
    return static_cast<const ProbabilisticTask*>(task)->get_operator_cost(
        index);
}

ProbabilisticOperatorsProxy::ProbabilisticOperatorsProxy(
    const ProbabilisticTask& task)
    : task_(&task)
{
}

std::size_t ProbabilisticOperatorsProxy::size() const
{
    return task_->get_num_operators();
}

ProbabilisticOperatorProxy
ProbabilisticOperatorsProxy::operator[](std::size_t index) const
{
    assert(index < size());
    return ProbabilisticOperatorProxy(*task_, index);
}

ProbabilisticOperatorProxy
ProbabilisticOperatorsProxy::operator[](OperatorID id) const
{
    return (*this)[id.get_index()];
}

ProbabilisticTaskProxy::ProbabilisticTaskProxy(const ProbabilisticTask& task)
    : PlanningTaskProxy(task)
{
}

ProbabilisticOperatorsProxy ProbabilisticTaskProxy::get_operators() const
{
    return ProbabilisticOperatorsProxy(
        *static_cast<const ProbabilisticTask*>(task));
}

const causal_graph::ProbabilisticCausalGraph&
ProbabilisticTaskProxy::get_causal_graph() const
{
    return causal_graph::get_causal_graph(
        static_cast<const ProbabilisticTask*>(task));
}

bool does_fire(const ProbabilisticEffectProxy& effect, const State& state)
{
    return std::ranges::all_of(
        effect.get_conditions(),
        [&](FactProxy condition) {
            return state[condition.get_variable()] == condition.get_value();
        });
}

} // namespace probfd