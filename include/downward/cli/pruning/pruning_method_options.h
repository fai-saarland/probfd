#ifndef DOWNWARD_PLUGINS_PRUNING_METHOD_H
#define DOWNWARD_PLUGINS_PRUNING_METHOD_H

#include <tuple>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace downward::cli {

extern std::size_t add_pruning_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli

#endif
