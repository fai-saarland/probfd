#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SUBTASK_GENERATORS_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SUBTASK_GENERATORS_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::cartesian_abstractions {

void add_subtask_generator_category(
    downward::cli::plugins::Registry& raw_registry);

void add_subtask_generator_features(
    downward::cli::plugins::Registry& raw_registry);

}

#endif