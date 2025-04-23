#ifndef PDBS_VALIDATION_H
#define PDBS_VALIDATION_H

#include "downward/pdbs/types.h"

namespace downward {
class AbstractTask;
}

namespace downward::utils {
class LogProxy;
}

namespace downward::pdbs {
extern void validate_and_normalize_pattern(
    const AbstractTask& task,
    Pattern& pattern,
    utils::LogProxy& log);
extern void validate_and_normalize_patterns(
    const AbstractTask& task,
    PatternCollection& patterns,
    utils::LogProxy& log);
} // namespace pdbs

#endif
