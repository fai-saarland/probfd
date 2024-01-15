#include "probfd/tasks/modified_goals_task.h"

#include <utility>

using namespace std;

namespace probfd {
namespace extra_tasks {

ModifiedGoalsTask::ModifiedGoalsTask(
    const shared_ptr<ProbabilisticTask>& parent,
    vector<FactPair>&& goals)
    : DelegatingTask(parent)
    , goals(std::move(goals))
{
}

int ModifiedGoalsTask::get_num_goals() const
{
    return goals.size();
}

FactPair ModifiedGoalsTask::get_goal_fact(int index) const
{
    return goals[index];
}

} // namespace extra_tasks
} // namespace probfd