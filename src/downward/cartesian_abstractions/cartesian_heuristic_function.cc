#include "downward/cartesian_abstractions/cartesian_heuristic_function.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include "downward/task_proxy.h"

#include "downward/utils/collections.h"

using namespace std;

namespace downward::cartesian_abstractions {
CartesianHeuristicFunction::CartesianHeuristicFunction(
    std::shared_ptr<AbstractTask> task,
    unique_ptr<RefinementHierarchy>&& hierarchy,
    vector<int>&& h_values)
    : task(std::move(task))
    , refinement_hierarchy(std::move(hierarchy))
    , h_values(std::move(h_values))
{
}

int CartesianHeuristicFunction::get_value(const State& state) const
{
    PlanningTaskProxy subtask_proxy(*task);
    State subtask_state = subtask_proxy.convert_ancestor_state(state);
    int abstract_state_id =
        refinement_hierarchy->get_abstract_state_id(subtask_state);
    assert(utils::in_bounds(abstract_state_id, h_values));
    return h_values[abstract_state_id];
}
} // namespace cartesian_abstractions
