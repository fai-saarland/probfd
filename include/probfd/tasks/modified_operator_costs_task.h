#ifndef PROBFD_TASKS_MODIFIED_OPERATOR_COSTS_TASK_H
#define PROBFD_TASKS_MODIFIED_OPERATOR_COSTS_TASK_H

#include "probfd/tasks/delegating_task.h" // IWYU pragma: export

#include "probfd/value_type.h"

#include <memory>
#include <vector>

namespace probfd::extra_tasks {

class ModifiedOperatorCostsTask final : public tasks::DelegatingTask {
    std::vector<value_t> operator_costs_;

public:
    ModifiedOperatorCostsTask(
        const std::shared_ptr<ProbabilisticTask>& parent,
        std::vector<value_t> costs);
    ~ModifiedOperatorCostsTask() override = default;

    value_t get_operator_cost(int index) const override;
    void set_operator_cost(int index, value_t cost);
};

} // namespace probfd::extra_tasks

#endif
