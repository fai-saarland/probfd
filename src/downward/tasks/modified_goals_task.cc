#include "downward/tasks/modified_goals_task.h"

using namespace std;

namespace downward::extra_tasks {

ModifiedGoalFacts::ModifiedGoalFacts(
    vector<FactPair> goals)
    : goals(std::move(goals))
{
}

int ModifiedGoalFacts::get_num_goals() const
{
    return goals.size();
}

FactPair ModifiedGoalFacts::get_goal_fact(int index) const
{
    return goals[index];
}

} // namespace downward::extra_tasks
