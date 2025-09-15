#ifndef DOWNWARD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_FEATURE_H
#define DOWNWARD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::pdbs {

void add_pattern_collection_generator_disjoint_cegar_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif