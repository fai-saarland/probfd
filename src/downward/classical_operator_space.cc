#include "downward/classical_operator_space.h"

#include "downward/task_utils/task_properties.h"

namespace downward {

State OperatorProxy::get_unregistered_successor(
    const State& state,
    AxiomEvaluator& axiom_evaluator) const
{
    assert(task_properties::is_applicable(*this, state));
    return state.get_unregistered_successor(axiom_evaluator, get_effects());
}

/*
void ClassicalOperatorProxy::apply_effect(std::vector<int>& values) const
{
    return state.get_unregistered_successor(task, get_effects());
}
*/

}