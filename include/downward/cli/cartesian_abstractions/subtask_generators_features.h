#ifndef PROBFD_SUBTASK_GENERATORS_FEATURES_H
#define PROBFD_SUBTASK_GENERATORS_FEATURES_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::cartesian_abstractions {

void add_subtask_generators_features(
    downward::cli::plugins::RawRegistry& raw_registry);

} // namespace downward::cli::cartesian_abstractions

#endif