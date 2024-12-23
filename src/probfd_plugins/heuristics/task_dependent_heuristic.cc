#include "probfd_plugins/heuristics/task_dependent_heuristic.h"

#include "downward_plugins/utils/logging_options.h"

using namespace downward_plugins::plugins;

using downward_plugins::utils::add_log_options_to_feature;
using downward_plugins::utils::get_log_arguments_from_options;

namespace probfd_plugins::heuristics {

void add_task_dependent_heuristic_options_to_feature(Feature& feature)
{
    add_log_options_to_feature(feature);
}

std::tuple<utils::Verbosity>
get_task_dependent_heuristic_arguments_from_options(const Options& opts)
{
    return get_log_arguments_from_options(opts);
}

} // namespace probfd_plugins::heuristics
