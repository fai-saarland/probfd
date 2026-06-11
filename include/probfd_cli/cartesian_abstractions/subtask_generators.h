#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SUBTASK_GENERATORS_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SUBTASK_GENERATORS_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::cartesian_abstractions {

void add_subtask_generator_features(
    language::plugins::RawRegistry& raw_registry);

}

#endif