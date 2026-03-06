#ifndef DOWNWARD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
#define DOWNWARD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace downward::cli::pdbs {

extern void add_multiple_algorithm_implementation_notes_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature);

extern std::size_t add_multiple_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::pdbs

#endif
