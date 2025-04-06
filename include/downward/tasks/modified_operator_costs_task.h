#ifndef TASKS_MODIFIED_OPERATOR_COSTS_TASK_H
#define TASKS_MODIFIED_OPERATOR_COSTS_TASK_H

#include "downward/tasks/delegating_task.h"

#include <vector>

namespace downward::extra_tasks {
class ModifiedOperatorCostsTask : public tasks::DelegatingTask {
    const std::vector<int> operator_costs;

public:
    ModifiedOperatorCostsTask(
        const std::shared_ptr<AbstractTask>& parent,
        std::vector<int>&& costs);
    virtual ~ModifiedOperatorCostsTask() override = default;

    virtual int get_operator_cost(int index) const override;
};
} // namespace extra_tasks

#endif
