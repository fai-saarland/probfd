#ifndef DOWNWARD_PLUGINS_HEURISTIC_H
#define DOWNWARD_PLUGINS_HEURISTIC_H

#include <cstddef>
#include <string>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
} // namespace downward::cli::plugins

namespace downward::cli {

extern std::size_t add_heuristic_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    const std::string& description,
    std::size_t start_index);

} // namespace downward::cli

#endif
