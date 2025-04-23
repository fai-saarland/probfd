#include "probfd/tasks/modified_operator_costs_task.h"

#include <cassert>
#include <ranges>
#include <utility>

using namespace std;

using namespace downward;

namespace probfd::extra_tasks {

ModifiedOperatorCostsTask::ModifiedOperatorCostsTask(
    const shared_ptr<ProbabilisticTask>& parent,
    vector<value_t> costs)
    : DelegatingTask(parent)
    , operator_costs_(std::move(costs))
{
    assert(static_cast<int>(operator_costs_.size()) == get_num_operators());
}

value_t ModifiedOperatorCostsTask::get_operator_cost(int index) const
{
    return operator_costs_[index];
}

void ModifiedOperatorCostsTask::set_operator_cost(int index, value_t cost)
{
    operator_costs_[index] = cost;
}

void ModifiedOperatorCostsTask::decrease_costs(const std::vector<value_t>& by)
{
    assert(by.size() == operator_costs_.size());

    for (const auto& [val, dec] : std::views::zip(operator_costs_, by)) {
        val -= dec;
    }
}

} // namespace probfd::extra_tasks
