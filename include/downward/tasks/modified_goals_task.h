#ifndef TASKS_MODIFIED_GOALS_TASK_H
#define TASKS_MODIFIED_GOALS_TASK_H

#include "downward/goal_fact_list.h"

#include <vector>

namespace downward::extra_tasks {
class ModifiedGoalFacts : public GoalFactList {
    const std::vector<FactPair> goals;

public:
    explicit ModifiedGoalFacts(std::vector<FactPair> goals);

    int get_num_goals() const override;
    FactPair get_goal_fact(int index) const override;
};
} // namespace downward::extra_tasks

#endif
