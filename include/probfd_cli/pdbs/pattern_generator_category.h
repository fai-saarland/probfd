#ifndef PROBFD_CLI_PDBS_PATTERN_GENERATOR_CATEGORY_H
#define PROBFD_CLI_PDBS_PATTERN_GENERATOR_CATEGORY_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::pdbs {

void add_pattern_generator_category(
    language::plugins::RawRegistry& raw_registry);

}

#endif