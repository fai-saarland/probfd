#include <utility>

#include "probfd/heuristics/task_dependent_heuristic.h"

namespace probfd::heuristics {

TaskDependentHeuristic::TaskDependentHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    utils::LogProxy log)
    : task_(task)
    , task_proxy_(*task)
    , log_(std::move(log))
{
}

void TaskDependentHeuristic::add_options_to_feature(plugins::Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

} // namespace probfd::heuristics
