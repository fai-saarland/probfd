#include "probfd_cli/heuristics/task_dependent_heuristic_options.h"

#include "downward_cli/utils/logging_options.h"

using namespace language;
using namespace language::plugins;

using downward::cli::utils::add_log_options_to_feature;
using downward::cli::utils::get_log_arguments_from_options;

namespace probfd::cli::heuristics {

void add_task_dependent_heuristic_options_to_feature(Feature&)
{
}

TaskDependentHeuristicArgs get_task_dependent_heuristic_arguments_from_options(
    const Context&,
    const Options&)
{
    return {};
}

} // namespace probfd::cli::heuristics
