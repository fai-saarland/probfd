#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_H

#include <cstddef>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
} // namespace downward::cli::plugins

namespace downward::cli::merge_and_shrink {

extern std::size_t add_merge_strategy_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::merge_and_shrink

#endif
