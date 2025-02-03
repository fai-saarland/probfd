#include "probfd/cartesian_abstractions/heuristics.h"

#ifndef NDEBUG
#include "downward/utils/collections.h"
#endif

#include <cassert>

namespace probfd::cartesian_abstractions {

value_t CartesianHeuristic::evaluate(int state) const
{
    return get_h_value(state);
}

value_t CartesianHeuristic::get_h_value(int state) const
{
    assert(utils::in_bounds(state, h_values_));
    return h_values_[state];
}

void CartesianHeuristic::set_h_value(int state, value_t h)
{
    h_values_[state] = h;
}

void CartesianHeuristic::on_split(int state)
{
    h_values_.push_back(h_values_[state]);
}

} // namespace probfd::cartesian_abstractions
