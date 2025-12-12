#ifndef DOWNWARD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MANUAL_FEATURE_H
#define DOWNWARD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MANUAL_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::pdbs {

void add_pattern_collection_generator_manual_feature(
    language::plugins::Registry& registry);

}

#endif