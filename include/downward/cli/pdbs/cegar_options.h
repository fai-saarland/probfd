#ifndef DOWNWARD_PLUGINS_PDBS_CEGAR_H
#define DOWNWARD_PLUGINS_PDBS_CEGAR_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace downward::cli::pdbs {

extern void
add_cegar_implementation_notes_to_feature(language::parser::InternalFunctionDefinitionBase& feature);

extern std::size_t add_cegar_wildcard_option_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::pdbs

#endif
