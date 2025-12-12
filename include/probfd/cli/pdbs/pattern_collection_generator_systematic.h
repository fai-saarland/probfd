#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_SYSTEMATIC_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_SYSTEMATIC_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_systematic_feature(
    language::plugins::Registry& registry);

}

#endif