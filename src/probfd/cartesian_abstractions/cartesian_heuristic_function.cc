#include "probfd/cartesian_abstractions/cartesian_heuristic_function.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include "downward/utils/collections.h"

#include <cassert>
#include <utility>

using namespace std;

namespace probfd::cartesian_abstractions {

CartesianHeuristicFunction::CartesianHeuristicFunction(
    unique_ptr<RefinementHierarchy>&& hierarchy,
    vector<value_t>&& h_values)
    : refinement_hierarchy_(std::move(hierarchy))
    , h_values_(std::move(h_values))
{
}

CartesianHeuristicFunction::~CartesianHeuristicFunction() = default;

value_t CartesianHeuristicFunction::get_value(const State& state) const
{
    int abstract_state_id = refinement_hierarchy_->get_abstract_state_id(state);
    assert(utils::in_bounds(abstract_state_id, h_values_));
    return h_values_[abstract_state_id];
}

} // namespace probfd::cartesian_abstractions
