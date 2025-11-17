#ifndef DOWNWARD_PLUGINS_EVALUATOR_H
#define DOWNWARD_PLUGINS_EVALUATOR_H

#include <cstddef>
#include <string>

namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli {

extern std::size_t add_evaluator_options_to_feature(
    plugins::Feature& feature,
    const std::string& description,
    std::size_t start_index);

} // namespace downward::cli

#endif
