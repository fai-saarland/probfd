#ifndef DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_HEURISTIC_H
#define DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_HEURISTIC_H

#include <memory>
#include <string>
#include <tuple>

class AbstractTask;

namespace utils {
enum class Verbosity;
}

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace landmarks {
class LandmarkFactory;
}

namespace downward_plugins::landmarks {

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
    bool,
    std::string,
    utils::Verbosity>
get_landmark_heuristic_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins::landmarks

#endif
