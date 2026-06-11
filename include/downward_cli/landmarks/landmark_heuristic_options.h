#ifndef DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_HEURISTIC_H
#define DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_HEURISTIC_H

#include <memory>
#include <string>
#include <tuple>

namespace downward {
class TaskTransformation;
} // namespace downward

namespace downward::utils {
enum class Verbosity;
}

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace downward::landmarks {
class LandmarkFactory;
}

namespace downward::cli::landmarks {

extern void add_landmark_heuristic_options_to_feature(
    language::plugins::Feature& feature,
    const std::string& description);

extern std::tuple<
    std::shared_ptr<TaskTransformation>,
    bool,
    std::string,
    utils::Verbosity,
    std::shared_ptr<downward::landmarks::LandmarkFactory>,
    bool,
    bool,
    bool,
    bool>
get_landmark_heuristic_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli::landmarks

#endif
