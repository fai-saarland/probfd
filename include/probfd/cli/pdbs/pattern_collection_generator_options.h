#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_H

#include <cstddef>

namespace language::plugins {
class InternalFunctionDefinitionBase;
} // namespace language::plugins

namespace probfd::cli::pdbs {

extern std::size_t add_pattern_collection_generator_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace probfd::cli::pdbs

#endif // PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_H
