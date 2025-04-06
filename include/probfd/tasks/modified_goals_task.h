#ifndef PROBFD_TASKS_MODIFIED_GOALS_TASK_H
#define PROBFD_TASKS_MODIFIED_GOALS_TASK_H

#include "probfd/tasks/delegating_task.h" // IWYU pragma: export

#include <memory>
#include <vector>

namespace probfd::extra_tasks {

class ModifiedGoalsTask final : public tasks::DelegatingTask {
    const std::vector<downward::FactPair> goals_;

public:
    ModifiedGoalsTask(
        const std::shared_ptr<ProbabilisticTask>& parent,
        std::vector<downward::FactPair>&& goals);
    ~ModifiedGoalsTask() override = default;

    int get_num_goals() const override;
    downward::FactPair get_goal_fact(int index) const override;
};

} // namespace probfd::extra_tasks

#endif
