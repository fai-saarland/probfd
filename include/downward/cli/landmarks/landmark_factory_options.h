#ifndef DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_FACTORY_H
#define DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_FACTORY_H

#include <cstddef>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
} // namespace downward::cli::plugins

namespace downward::cli::landmarks {

extern std::size_t add_landmark_factory_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

extern std::size_t add_use_orders_option_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::landmarks

#endif
