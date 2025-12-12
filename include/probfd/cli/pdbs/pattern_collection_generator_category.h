#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_CATEGORY_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_CATEGORY_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_category(
    language::plugins::Registry& registry);

}

#endif