#include "downward/classical_operator_space.h"

#include "downward/state_registry.h"

#include "downward/task_utils/task_properties.h"

namespace downward {

State OperatorProxy::get_unregistered_successor(
    const State& state,
    AxiomEvaluator& axiom_evaluator) const
{
    assert(task_properties::is_applicable(*this, state));
    return state.get_unregistered_successor(axiom_evaluator, get_effects());
}

State OperatorProxy::get_registered_successor(
    const State& state,
    StateRegistry& registry) const
{
    return registry.get_successor_state(state, get_effects());
}

}