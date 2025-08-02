#ifndef PROBFD_CLI_PDBS_PATTERN_GENERATOR_CATEGORY_H
#define PROBFD_CLI_PDBS_PATTERN_GENERATOR_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::pdbs {

void add_pattern_generator_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif