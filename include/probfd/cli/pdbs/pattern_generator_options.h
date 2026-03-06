#ifndef PROBFD_CLI_PDBS_PATTERN_GENERATOR_H
#define PROBFD_CLI_PDBS_PATTERN_GENERATOR_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace probfd::cli::pdbs {

extern std::size_t add_pattern_generator_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace probfd::cli::pdbs

#endif // PROBFD_CLI_PDBS_PATTERN_GENERATOR_H
