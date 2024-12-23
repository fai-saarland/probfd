#ifndef PROBFD_PLUGINS_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H
#define PROBFD_PLUGINS_HEURISTICS_TASK_DEPENDENT_HEURISTIC_H

#include <tuple>

// Forward Declarations
namespace utils {
enum class Verbosity;
}

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace probfd_plugins::heuristics {

extern void add_task_dependent_heuristic_options_to_feature(
    downward_plugins::plugins::Feature& feature);

extern std::tuple<utils::Verbosity>
get_task_dependent_heuristic_arguments_from_options(
    const downward_plugins::plugins::Options& opts);

} // namespace probfd_plugins::heuristics

#endif