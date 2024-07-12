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

void add_task_dependent_heuristic_options_to_feature(plugins::Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

std::tuple<utils::Verbosity>
get_task_dependent_heuristic_arguments_from_options(
    const plugins::Options& opts)
{
    return utils::get_log_arguments_from_options(opts);
}

} // namespace probfd::heuristics
