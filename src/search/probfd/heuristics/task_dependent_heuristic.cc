#include "probfd/heuristics/task_dependent_heuristic.h"

#include "option_parser.h"

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

TaskDependentHeuristic::TaskDependentHeuristic(const options::Options& options)
    : TaskDependentHeuristic(
          options.get<std::shared_ptr<ProbabilisticTask>>("transform"),
          utils::get_log_from_options(options))

{
}

void TaskDependentHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<ProbabilisticTask>>(
        "transform",
        "Optional task transformation for the heuristic. "
        "Currently, only root_ppt() is available.",
        "root_ppt()");
    utils::add_log_options_to_parser(parser);
}

} // namespace heuristics
} // namespace probfd