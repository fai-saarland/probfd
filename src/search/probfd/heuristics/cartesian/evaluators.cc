#include "probfd/heuristics/cartesian/evaluators.h"

#include "probfd/heuristics/cartesian/abstract_state.h"

#include "downward/utils/collections.h"

namespace probfd {
namespace heuristics {
namespace cartesian {

value_t CartesianHeuristic::evaluate(AbstractStateIndex state) const
{
    return h_values[state];
}

value_t& CartesianHeuristic::operator[](AbstractStateIndex state)
{
    return h_values[state];
}

const value_t& CartesianHeuristic::operator[](AbstractStateIndex state) const
{
    return h_values[state];
}

void CartesianHeuristic::on_split(AbstractStateIndex state)
{
    h_values.push_back(h_values[state]);
}

} // namespace cartesian
} // namespace heuristics
} // namespace probfd
