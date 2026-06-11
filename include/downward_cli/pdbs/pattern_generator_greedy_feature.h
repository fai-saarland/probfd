#ifndef DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_GREEDY_FEATURE_H
#define DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_GREEDY_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::pdbs {

void add_pattern_generator_greedy_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif