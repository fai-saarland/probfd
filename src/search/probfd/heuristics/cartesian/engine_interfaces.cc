#include "probfd/heuristics/cartesian/engine_interfaces.h"

#include "probfd/heuristics/cartesian/abstract_state.h"

#include "downward/utils/collections.h"

namespace probfd {
namespace heuristics {
namespace cartesian {

value_t CartesianHeuristic::evaluate(const AbstractState* state) const
{
    assert(utils::in_bounds(state->get_id(), h_values));
    return h_values[state->get_id()];
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
