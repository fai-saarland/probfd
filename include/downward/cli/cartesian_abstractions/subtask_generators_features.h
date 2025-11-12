#ifndef DOWNWARD_SUBTASK_GENERATORS_FEATURES_H
#define DOWNWARD_SUBTASK_GENERATORS_FEATURES_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::cartesian_abstractions {

void add_subtask_generators_features(
    downward::cli::plugins::Registry& raw_registry);

} // namespace downward::cli::cartesian_abstractions

#endif