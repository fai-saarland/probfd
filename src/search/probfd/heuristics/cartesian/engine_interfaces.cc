#include "probfd/heuristics/cartesian/engine_interfaces.h"

#include "probfd/heuristics/cartesian/abstract_state.h"
#include "probfd/heuristics/cartesian/abstraction.h"
#include "probfd/heuristics/cartesian/probabilistic_transition.h"

namespace probfd {
namespace heuristics {
namespace cartesian {

CartesianCostFunction::CartesianCostFunction(
    Abstraction& abstraction,
    std::vector<value_t> operator_costs)
    : abstraction(abstraction)
    , operator_costs(std::move(operator_costs))
{
}

TerminationInfo
CartesianCostFunction::get_termination_info(const AbstractState* s)
{
    if (abstraction.get_goals().contains(s->get_id())) {
        return TerminationInfo(true, 0.0_vt);
    }

    return TerminationInfo(false, INFINITE_VALUE);
}

value_t CartesianCostFunction::get_action_cost(const ProbabilisticTransition* t)
{
    return operator_costs[t->op_id];
}

value_t CartesianCostFunction::get_cost(int op_index) const
{
    return operator_costs[op_index];
}

EvaluationResult
CartesianHeuristic::evaluate(const AbstractState* state) const
{
    assert(utils::in_bounds(state->get_id(), h_values));
    const value_t h = h_values[state->get_id()];
    return EvaluationResult(h == INFINITE_VALUE, h);
}

value_t CartesianHeuristic::get_h_value(int v) const
{
    return h_values[v];
}

void CartesianHeuristic::set_h_value(int v, value_t h)
{
    h_values[v] = h;
}

void CartesianHeuristic::on_split(int v)
{
    h_values.push_back(h_values[v]);
}

} // namespace cartesian
} // namespace heuristics
} // namespace probfd
