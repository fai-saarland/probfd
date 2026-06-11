#ifndef PROBFD_SUBTASK_GENERATORS_FEATURES_H
#define PROBFD_SUBTASK_GENERATORS_FEATURES_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::cartesian_abstractions {

void add_subtask_generators_features(
    language::plugins::RawRegistry& raw_registry);

} // namespace downward::cli::cartesian_abstractions

#endif