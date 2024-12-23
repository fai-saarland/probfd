#ifndef PROBFD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H
#define PROBFD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H

// Forward Declarations
namespace downward_plugins::plugins {
class Feature;
}

namespace probfd_plugins::pdbs {

void add_pattern_collection_generator_cegar_options_to_feature(
    downward_plugins::plugins::Feature& feature);

} // namespace probfd_plugins::pdbs

#endif
