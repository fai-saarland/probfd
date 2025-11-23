#ifndef PROBFD_CLI_PDBS_PATTERN_GENERATOR_H
#define PROBFD_CLI_PDBS_PATTERN_GENERATOR_H

#include <cstddef>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
} // namespace downward::cli::plugins

namespace probfd::cli::pdbs {

extern std::size_t add_pattern_generator_options_to_feature(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace probfd::cli::pdbs

#endif // PROBFD_CLI_PDBS_PATTERN_GENERATOR_H
