#ifndef PROBFD_CLI_HEURISTICS_TASK_DEPENDENT_HEURISTIC_OPTIONS_H
#define PROBFD_CLI_HEURISTICS_TASK_DEPENDENT_HEURISTIC_OPTIONS_H

#include "probfd_cli/heuristics/task_dependent_heuristic_options_fwd.h"

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace probfd::cli::heuristics {

extern void add_task_dependent_heuristic_options_to_feature(
    language::plugins::Feature& feature);

extern TaskDependentHeuristicArgs
get_task_dependent_heuristic_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace probfd::cli::heuristics

#endif // PROBFD_CLI_HEURISTICS_TASK_DEPENDENT_HEURISTIC_OPTIONS_H
