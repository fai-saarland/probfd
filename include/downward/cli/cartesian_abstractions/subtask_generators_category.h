#ifndef DOWNWARD_SUBTASK_GENERATORS_CATEGORY_H
#define DOWNWARD_SUBTASK_GENERATORS_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::cartesian_abstractions {

void add_subtask_generator_category(
    downward::cli::plugins::RawRegistry& raw_registry);

} // namespace downward::cli::cartesian_abstractions

#endif
