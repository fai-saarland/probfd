#include "probfd/cartesian/cartesian_heuristic_function.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include "downward/utils/collections.h"

using namespace std;

namespace probfd {
namespace cartesian {

CartesianHeuristicFunction::CartesianHeuristicFunction(
    unique_ptr<RefinementHierarchy>&& hierarchy,
    vector<value_t>&& h_values)
    : refinement_hierarchy(std::move(hierarchy))
    , h_values(std::move(h_values))
{
}

CartesianHeuristicFunction::~CartesianHeuristicFunction() = default;

value_t CartesianHeuristicFunction::get_value(const State& state) const
{
    int abstract_state_id = refinement_hierarchy->get_abstract_state_id(state);
    assert(utils::in_bounds(abstract_state_id, h_values));
    return h_values[abstract_state_id];
}

} // namespace cartesian
} // namespace probfd