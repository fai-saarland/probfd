#include "probfd/heuristics/task_dependent_heuristic.h"

#include <utility>

using namespace downward;

namespace probfd::heuristics {

TaskDependentHeuristic::TaskDependentHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    utils::LogProxy log)
    : task_(task)
    , task_proxy_(*task)
    , log_(std::move(log))
{
}

} // namespace probfd::heuristics
