#ifndef DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_HEURISTIC_H
#define DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_HEURISTIC_H

#include <memory>
#include <string>
#include <tuple>

class AbstractTask;
class TaskTransformation;

namespace utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace landmarks {
class LandmarkFactory;
}

namespace downward::cli::landmarks {

extern void add_landmark_heuristic_options_to_feature(
    plugins::Feature& feature,
    const std::string& description);

extern std::tuple<
    std::shared_ptr<::landmarks::LandmarkFactory>,
    bool,
    bool,
    bool,
    bool,
    std::shared_ptr<AbstractTask>,
    std::shared_ptr<TaskTransformation>,
    bool,
    std::string,
    utils::Verbosity>
get_landmark_heuristic_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli::landmarks

#endif
