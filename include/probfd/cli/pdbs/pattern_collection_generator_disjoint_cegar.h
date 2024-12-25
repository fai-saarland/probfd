#ifndef PROBFD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H
#define PROBFD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H

// Forward Declarations
namespace downward::cli::plugins {
class Feature;
}

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_cegar_options_to_feature(
    downward::cli::plugins::Feature& feature);

} // namespace probfd::cli::pdbs

#endif
