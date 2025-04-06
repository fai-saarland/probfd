#include "probfd/tasks/modified_goals_task.h"

#include <utility>

using namespace std;

using namespace downward;

namespace probfd::extra_tasks {

ModifiedGoalsTask::ModifiedGoalsTask(
    const shared_ptr<ProbabilisticTask>& parent,
    vector<FactPair>&& goals)
    : DelegatingTask(parent)
    , goals_(std::move(goals))
{
}

int ModifiedGoalsTask::get_num_goals() const
{
    return static_cast<int>(goals_.size());
}

FactPair ModifiedGoalsTask::get_goal_fact(int index) const
{
    return goals_[index];
}

} // namespace probfd::extra_tasks
