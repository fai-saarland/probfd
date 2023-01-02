#include "probfd/heuristics/task_dependent_heuristic.h"

#include "option_parser.h"

namespace probfd {
namespace heuristics {

TaskDependentHeuristic::TaskDependentHeuristic(
    std::shared_ptr<ProbabilisticTask> task)
    : task(task)
    , task_proxy(*task)
{
}

TaskDependentHeuristic::TaskDependentHeuristic(const options::Options& options)
    : TaskDependentHeuristic(
        options.get<std::shared_ptr<ProbabilisticTask>>("transform"))
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
}

} // namespace heuristics
} // namespace probfd