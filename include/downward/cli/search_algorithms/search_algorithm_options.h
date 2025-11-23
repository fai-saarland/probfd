#ifndef DOWNWARD_PLUGINS_SEARCH_ALGORITHM_H
#define DOWNWARD_PLUGINS_SEARCH_ALGORITHM_H

#include <cstddef>
#include <memory>
#include <string>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
} // namespace downward::cli::plugins

namespace downward::cli {

extern std::size_t add_search_pruning_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

extern std::size_t add_search_algorithm_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    const std::string& description,
    std::size_t start_index);

extern std::size_t add_successors_order_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli

#endif
