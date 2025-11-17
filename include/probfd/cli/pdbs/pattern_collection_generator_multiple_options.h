#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H

#include <cstddef>

// Forward Declarations
namespace downward::cli::plugins {
class Feature;
} // namespace downward::cli::plugins

namespace probfd::cli::pdbs {

extern std::size_t add_multiple_options_to_feature(
    downward::cli::plugins::Feature& feature,
    std::size_t start_index);

} // namespace probfd::cli::pdbs

#endif // PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
