#ifndef PDBS_VALIDATION_H
#define PDBS_VALIDATION_H

#include "downward/pdbs/types.h"

namespace downward {
class TaskProxy;
}

namespace downward::utils {
class LogProxy;
}

namespace downward::pdbs {
extern void validate_and_normalize_pattern(
    const TaskProxy& task_proxy,
    Pattern& pattern,
    utils::LogProxy& log);
extern void validate_and_normalize_patterns(
    const TaskProxy& task_proxy,
    PatternCollection& patterns,
    utils::LogProxy& log);
} // namespace pdbs

#endif
