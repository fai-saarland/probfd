#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_POLICY_BASED_FLAW_GENERATOR_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_POLICY_BASED_FLAW_GENERATOR_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::cartesian_abstractions {

void add_policy_based_flaw_generator_feature(
    language::plugins::Registry& registry);

}

#endif