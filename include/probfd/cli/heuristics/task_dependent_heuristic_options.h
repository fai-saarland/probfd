#ifndef CLI_HEURISTICS_TASK_DEPENDENT_HEURISTIC_OPTIONS_H
#define CLI_HEURISTICS_TASK_DEPENDENT_HEURISTIC_OPTIONS_H

#include "probfd/cli/heuristics/task_dependent_heuristic_options_fwd.h"

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace probfd::cli::heuristics {

extern void add_task_dependent_heuristic_options_to_feature(
    downward::cli::plugins::Feature& feature);

extern TaskDependentHeuristicArgs
get_task_dependent_heuristic_arguments_from_options(
    const downward::cli::plugins::Options& opts);

} // namespace probfd::cli::heuristics

#endif // CLI_HEURISTICS_TASK_DEPENDENT_HEURISTIC_OPTIONS_H
