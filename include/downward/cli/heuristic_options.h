#ifndef DOWNWARD_PLUGINS_HEURISTIC_H
#define DOWNWARD_PLUGINS_HEURISTIC_H

#include <memory>
#include <string>
#include <tuple>

namespace downward {
class AbstractTask;
class TaskTransformation;
}

namespace downward::utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::cli {

extern void add_heuristic_options_to_feature(
    plugins::Feature& feature,
    const std::string& description);

extern std::tuple<
    std::shared_ptr<AbstractTask>,
    std::shared_ptr<TaskTransformation>,
    bool,
    std::string,
    utils::Verbosity>
get_heuristic_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli

#endif
