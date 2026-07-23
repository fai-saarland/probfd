#include "probfd/heuristics/task_dependent_heuristic.h"

#include <utility>

namespace probfd::heuristics {

TaskDependentHeuristic::TaskDependentHeuristic(SharedProbabilisticTask task)
    : task_(std::move(task))
{
}

} // namespace probfd::heuristics
