#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_FLAW_GENERATOR_CATEGORY_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_FLAW_GENERATOR_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::cartesian_abstractions {

void add_flaw_generator_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif