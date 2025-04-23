#include "downward/heuristics/goal_count_heuristic.h"

#include "downward/utils/logging.h"

#include "downward/task_transformation.h"

#include <iostream>

using namespace std;

namespace downward::goal_count_heuristic {
GoalCountHeuristic::GoalCountHeuristic(
    std::shared_ptr<AbstractTask> original_task,
    TaskTransformationResult transformation_result,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(
          std::move(original_task),
          std::move(transformation_result),
          cache_estimates,
          description,
          verbosity)
{
    if (log.is_at_least_normal()) {
        log << "Initializing goal count heuristic..." << endl;
    }
}

GoalCountHeuristic::GoalCountHeuristic(
    std::shared_ptr<AbstractTask> original_task,
    const std::shared_ptr<TaskTransformation>& transformation,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : GoalCountHeuristic(
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          description,
          verbosity)
{
}

int GoalCountHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    int unsatisfied_goal_count = 0;

    for (const auto [var, value] : transformed_task->get_goals()) {
        if (state[var] != value) {
            ++unsatisfied_goal_count;
        }
    }
    return unsatisfied_goal_count;
}

} // namespace goal_count_heuristic
