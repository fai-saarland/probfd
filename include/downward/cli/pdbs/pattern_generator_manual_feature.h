#ifndef DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_MANUAL_FEATURE_H
#define DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_MANUAL_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::pdbs {

void add_pattern_generator_manual_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif