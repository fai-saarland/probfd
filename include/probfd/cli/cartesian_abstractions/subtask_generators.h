#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SUBTASK_GENERATORS_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SUBTASK_GENERATORS_H

namespace language::plugins {
class Namespace;
}

namespace probfd::cli::cartesian_abstractions {

void add_subtask_generator_category(language::plugins::Namespace& nspace);

void add_subtask_generator_features(language::plugins::Namespace& nspace);

} // namespace probfd::cli::cartesian_abstractions

#endif