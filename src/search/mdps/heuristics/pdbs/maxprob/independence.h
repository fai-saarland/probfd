#pragma once

#include "../types.h"

namespace probabilistic {
namespace pdbs {

/// Namespace dedicated to MaxProb PDB multiplicativity constraints.
namespace multiplicativity {

bool is_independent_collection(const PatternCollection& patterns);

} // namespace multiplicativity
} // namespace pdbs
} // namespace probabilistic
