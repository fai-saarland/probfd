#ifndef DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_RANDOM_FEATURE_H
#define DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_RANDOM_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::pdbs {

void add_pattern_generator_random_feature(
    language::plugins::Registry& registry);

}

#endif