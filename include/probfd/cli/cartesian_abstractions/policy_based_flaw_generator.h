#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_POLICY_BASED_FLAW_GENERATOR_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_POLICY_BASED_FLAW_GENERATOR_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::cartesian_abstractions {

void add_policy_based_flaw_generator_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif