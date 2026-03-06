#ifndef DOWNWARD_PLUGINS_UTILS_LOGGING_H
#define DOWNWARD_PLUGINS_UTILS_LOGGING_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace downward::cli::utils {

extern std::size_t add_log_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::utils

#endif
