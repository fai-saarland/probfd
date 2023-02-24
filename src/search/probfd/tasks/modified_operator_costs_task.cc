#include "probfd/tasks/modified_operator_costs_task.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace extra_tasks {

ModifiedOperatorCostsTask::ModifiedOperatorCostsTask(
    const shared_ptr<ProbabilisticTask>& parent,
    vector<value_t> costs)
    : DelegatingTask(parent)
    , operator_costs(std::move(costs))
{
    assert(static_cast<int>(operator_costs.size()) == get_num_operators());
}

value_t ModifiedOperatorCostsTask::get_operator_cost(int index) const
{
    return operator_costs[index];
}

void ModifiedOperatorCostsTask::set_operator_cost(int index, value_t cost)
{
    operator_costs[index] = cost;
}

} // namespace extra_tasks
} // namespace probfd