#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_HILLCLIMBING_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_hillclimbing_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif