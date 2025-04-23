#include "downward/heuristics/blind_search_heuristic.h"

#include "downward/task_utils/task_properties.h"
#include "downward/utils/logging.h"

#include "downward/task_transformation.h"

#include <cstddef>
#include <limits>
#include <utility>

using namespace std;

namespace downward::blind_search_heuristic {
BlindSearchHeuristic::BlindSearchHeuristic(
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
    , min_operator_cost(
          task_properties::get_min_operator_cost(
              transformed_task->get_operators(),
              *transformed_task))
{
    if (log.is_at_least_normal()) {
        log << "Initializing blind search heuristic..." << endl;
    }
}

BlindSearchHeuristic::BlindSearchHeuristic(
    std::shared_ptr<AbstractTask> original_task,
    const std::shared_ptr<TaskTransformation>& transformation,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : BlindSearchHeuristic(
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          description,
          verbosity)
{
}

int BlindSearchHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    if (task_properties::is_goal_state(*transformed_task, state))
        return 0;
    else
        return min_operator_cost;
}

} // namespace downward::blind_search_heuristic
