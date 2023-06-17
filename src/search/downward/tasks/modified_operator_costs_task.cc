#include "downward/tasks/modified_operator_costs_task.h"

#include <cassert>

using namespace std;

namespace extra_tasks {
ModifiedOperatorCostsTask::ModifiedOperatorCostsTask(
    const shared_ptr<AbstractTask>& parent,
    vector<int>&& costs)
    : DelegatingTask(parent)
    , operator_costs(std::move(costs))
{
    assert(static_cast<int>(operator_costs.size()) == get_num_operators());
}

int ModifiedOperatorCostsTask::get_operator_cost(int index) const
{
    return operator_costs[index];
}
} // namespace extra_tasks
