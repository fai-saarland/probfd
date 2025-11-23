#ifndef DOWNWARD_PLUGINS_OPEN_LIST_FACTORY_H
#define DOWNWARD_PLUGINS_OPEN_LIST_FACTORY_H

#include <cstddef>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
} // namespace downward::cli::plugins

namespace downward::cli {

extern void add_open_list_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli

#endif
