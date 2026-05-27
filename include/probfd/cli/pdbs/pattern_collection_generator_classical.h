#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_CLASSICAL_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_CLASSICAL_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_classical_feature(
    downward::cli::plugins::Registry& registry);

}

#endif