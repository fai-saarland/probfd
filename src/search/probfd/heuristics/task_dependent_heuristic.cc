#include "probfd/heuristics/task_dependent_heuristic.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {

TaskDependentHeuristic::TaskDependentHeuristic(
    std::shared_ptr<ProbabilisticTask> task,
    utils::LogProxy log)
    : task(task)
    , task_proxy(*task)
    , log(log)
{
}

void TaskDependentHeuristic::add_options_to_feature(plugins::Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

} // namespace heuristics
} // namespace probfd