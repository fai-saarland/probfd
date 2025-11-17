#ifndef DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_HEURISTIC_H
#define DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_HEURISTIC_H

#include <cstddef>
#include <string>

namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli::landmarks {

extern std::size_t add_landmark_heuristic_options_to_feature(
    plugins::Feature& feature,
    const std::string& description,
    std::size_t start_index);

} // namespace downward::cli::landmarks

#endif
