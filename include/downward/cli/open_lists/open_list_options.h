#ifndef DOWNWARD_PLUGINS_OPEN_LIST_FACTORY_H
#define DOWNWARD_PLUGINS_OPEN_LIST_FACTORY_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace downward::cli {

extern void add_open_list_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli

#endif
