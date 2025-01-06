#ifndef PROBFD_TASKS_MODIFIED_GOALS_TASK_H
#define PROBFD_TASKS_MODIFIED_GOALS_TASK_H

#include "probfd/tasks/delegating_task.h" // IWYU pragma: export

#include <memory>
#include <vector>

namespace probfd::extra_tasks {

class ModifiedGoalsTask : public tasks::DelegatingTask {
    const std::vector<FactPair> goals_;

public:
    ModifiedGoalsTask(
        const std::shared_ptr<ProbabilisticTask>& parent,
        std::vector<FactPair>&& goals);
    ~ModifiedGoalsTask() override = default;

    int get_num_goals() const override;
    FactPair get_goal_fact(int index) const override;
};

} // namespace probfd::extra_tasks

#endif
