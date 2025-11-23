#ifndef DOWNWARD_PLUGINS_UTILS_LOGGING_H
#define DOWNWARD_PLUGINS_UTILS_LOGGING_H

#include <cstddef>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
} // namespace downward::cli::plugins

namespace downward::cli::utils {

extern std::size_t
add_log_options_to_feature(plugins::InternalFunctionDefinitionBase& feature, std::size_t start_index);

} // namespace downward::cli::utils

#endif
