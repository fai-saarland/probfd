#include "downward/cartesian_abstractions/cartesian_heuristic_function.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include "downward/task_proxy.h"
#include "downward/task_transformation.h"

#include "downward/utils/collections.h"

using namespace std;

namespace downward::cartesian_abstractions {
CartesianHeuristicFunction::CartesianHeuristicFunction(
    std::shared_ptr<StateMapping> state_mapping,
    std::shared_ptr<AbstractTask> transformed_task,
    unique_ptr<RefinementHierarchy>&& hierarchy,
    vector<int>&& h_values)
    : state_mapping(std::move(state_mapping))
    , transformed_task(std::move(transformed_task))
    , refinement_hierarchy(std::move(hierarchy))
    , h_values(std::move(h_values))
{
}

int CartesianHeuristicFunction::get_value(const State& state) const
{
    State subtask_state =
        state_mapping->convert_ancestor_state(state, *transformed_task);
    int abstract_state_id =
        refinement_hierarchy->get_abstract_state_id(subtask_state);
    assert(utils::in_bounds(abstract_state_id, h_values));
    return h_values[abstract_state_id];
}
} // namespace cartesian_abstractions
