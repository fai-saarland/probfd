#ifndef DOWNWARD_PLUGINS_HEURISTIC_H
#define DOWNWARD_PLUGINS_HEURISTIC_H

#include <cstddef>
#include <string>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace downward::cli {

extern std::size_t add_heuristic_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    const std::string& description,
    std::size_t start_index);

} // namespace downward::cli

#endif
