#ifndef DOWNWARD_SUBTASK_GENERATORS_FEATURES_H
#define DOWNWARD_SUBTASK_GENERATORS_FEATURES_H

namespace language::plugins {
class Namespace;
}

namespace downward::cli::cartesian_abstractions {

void add_subtask_generators_features(language::plugins::Namespace& nspace);

} // namespace downward::cli::cartesian_abstractions

#endif