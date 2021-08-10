#pragma once

#include "types.h"

#include <ostream>

namespace probabilistic {
namespace pdbs {

void dump_pattern_vars(std::ostream& out, const Pattern& p);
void dump_pattern_short(std::ostream& out, PatternID i, const Pattern& p);
void dump_pattern(std::ostream& out, PatternID i, const Pattern& p);

} // namespace pdbs
} // namespace probabilistic