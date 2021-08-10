#pragma once

#include "../types.h"

#include <vector>

namespace probabilistic {
namespace pdbs {

std::vector<std::vector<int>>
build_compatibility_graph_weak_orthogonality(const PatternCollection& patterns);

} // namespace pdbs
} // namespace probabilistic
