#include "probfd/heuristics/cartesian/evaluators.h"

#include "probfd/heuristics/cartesian/abstract_state.h"

#include "downward/utils/collections.h"

namespace probfd {
namespace heuristics {
namespace cartesian {

value_t CartesianHeuristic::evaluate(int state) const
{
    return get_h_value(state);
}

value_t CartesianHeuristic::get_h_value(int state) const
{
    assert(utils::in_bounds(state, h_values));
    return h_values[state];
}

void CartesianHeuristic::set_h_value(int state, value_t h)
{
    h_values[state] = h;
}

void CartesianHeuristic::on_split(int state)
{
    h_values.push_back(h_values[state]);
}

} // namespace cartesian
} // namespace heuristics
} // namespace probfd
