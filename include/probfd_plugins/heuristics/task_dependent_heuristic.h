#ifndef PROBFD_PLUGINS_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H
#define PROBFD_PLUGINS_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H

#include "downward/utils/logging.h"

#include <utility>

// Forward Declarations
namespace plugins {
class Feature;
class Options;
} // namespace plugins

namespace probfd_plugins::heuristics {

extern void
add_task_dependent_heuristic_options_to_feature(plugins::Feature& feature);

extern std::tuple<utils::Verbosity>
get_task_dependent_heuristic_arguments_from_options(
    const plugins::Options& opts);

} // namespace probfd_plugins::heuristics

#endif