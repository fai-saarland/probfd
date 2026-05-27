#include "downward/pdbs/validation.h"

#include "downward/abstract_task.h"
#include "downward/state.h"

#include "downward/utils/logging.h"
#include "downward/utils/system.h"

#include <algorithm>
#include <iostream>

using namespace std;
using downward::utils::ExitCode;

namespace downward::pdbs {
void validate_and_normalize_pattern(
    const VariableSpace& variables,
    Pattern& pattern,
    utils::LogProxy& log)
{
    /*
      - Sort by variable number and remove duplicate variables.
      - Warn if duplicate variables exist.
      - Error if patterns contain out-of-range variable numbers.
    */
    ranges::sort(pattern);

    if (const auto [it, end] = ranges::unique(pattern); it != end) {
        pattern.erase(it, pattern.end());
        if (log.is_warning()) {
            log << "Warning: duplicate variables in pattern have been removed"
                << endl;
        }
    }

    if (!pattern.empty()) {
        if (pattern.front() < 0) {
            throw utils::CriticalError("Variable number too low in pattern");
        }
        if (const int num_vars = variables.size(); pattern.back() >= num_vars) {
            throw utils::CriticalError("Variable number too high in pattern");
        }
    }
}

void validate_and_normalize_patterns(
    const VariableSpace& variables,
    PatternCollection& patterns,
    utils::LogProxy& log)
{
    /*
      - Validate and normalize each pattern (see there).
      - Warn if duplicate patterns exist.
    */
    for (Pattern& pattern : patterns)
        validate_and_normalize_pattern(variables, pattern, log);

    PatternCollection sorted_patterns(patterns);
    ranges::sort(sorted_patterns);

    if (auto [it, end] = ranges::unique(sorted_patterns); it != end) {
        if (log.is_warning()) {
            log << "Warning: duplicate patterns have been detected" << endl;
        }
    }
}
} // namespace downward::pdbs
