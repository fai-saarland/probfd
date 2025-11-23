#ifndef DOWNWARD_PLUGINS_SEARCH_ALGORITHMS_EAGER_SEARCH_H
#define DOWNWARD_PLUGINS_SEARCH_ALGORITHMS_EAGER_SEARCH_H

#include <cstddef>
#include <string>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
} // namespace downward::cli::plugins

namespace downward::cli::eager_search {

extern std::size_t add_eager_search_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    const std::string& description,
    std::size_t start_index);

} // namespace downward::cli::eager_search

#endif
