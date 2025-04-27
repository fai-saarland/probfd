#include "probfd/heuristics/task_dependent_heuristic.h"

#include <utility>

using namespace downward;

namespace probfd::heuristics {

TaskDependentHeuristic::TaskDependentHeuristic(
    SharedProbabilisticTask task,
    utils::LogProxy log)
    : task_(std::move(task))
    , log_(std::move(log))
{
}

} // namespace probfd::heuristics
