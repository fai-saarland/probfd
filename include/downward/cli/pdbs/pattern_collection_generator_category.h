#ifndef DOWNWARD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_CATEGORY_H
#define DOWNWARD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::pdbs {

void add_pattern_collection_generator_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif