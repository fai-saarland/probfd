#include "probfd/cartesian_abstractions/cartesian_heuristic_function.h"

#include "probfd/probabilistic_task.h"
#include "probfd/task_proxy.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include "downward/utils/collections.h"

#include "downward/task_transformation.h"

#include <cassert>
#include <utility>

using namespace std;
using namespace downward;

namespace probfd::cartesian_abstractions {

CartesianHeuristicFunction::CartesianHeuristicFunction(
    std::shared_ptr<StateMapping> state_mapping,
    std::shared_ptr<ProbabilisticTask> task,
    unique_ptr<RefinementHierarchy>&& hierarchy,
    vector<value_t>&& h_values)
    : state_mapping_(std::move(state_mapping))
    , task_(std::move(task))
    , refinement_hierarchy_(std::move(hierarchy))
    , h_values_(std::move(h_values))
{
}

CartesianHeuristicFunction::CartesianHeuristicFunction(
    CartesianHeuristicFunction&&) noexcept = default;

CartesianHeuristicFunction& CartesianHeuristicFunction::operator=(
    CartesianHeuristicFunction&&) noexcept = default;

CartesianHeuristicFunction::~CartesianHeuristicFunction() = default;

value_t CartesianHeuristicFunction::get_value(const State& state) const
{
    State subtask_state = state_mapping_->convert_ancestor_state(state, *task_);
    int abstract_state_id =
        refinement_hierarchy_->get_abstract_state_id(subtask_state);
    assert(utils::in_bounds(abstract_state_id, h_values_));
    return h_values_[abstract_state_id];
}

} // namespace probfd::cartesian_abstractions
