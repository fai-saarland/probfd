#ifndef DOWNWARD_PLUGINS_PDBS_RANDOM_PATTERN_H
#define DOWNWARD_PLUGINS_PDBS_RANDOM_PATTERN_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace downward::cli::pdbs {

extern void
add_random_pattern_implementation_notes_to_feature(language::parser::InternalFunctionDefinitionBase& feature);

extern std::size_t add_random_pattern_bidirectional_option_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::pdbs

#endif
