#ifndef PDBS_VALIDATION_H
#define PDBS_VALIDATION_H

#include "downward/pdbs/types.h"

namespace downward {
class VariableSpace;
}

namespace downward::utils {
class LogProxy;
}

namespace downward::pdbs {
extern void validate_and_normalize_pattern(
    const VariableSpace& variables,
    Pattern& pattern,
    utils::LogProxy& log);

extern void validate_and_normalize_patterns(
    const VariableSpace& variables,
    PatternCollection& patterns,
    utils::LogProxy& log);
} // namespace pdbs

#endif
