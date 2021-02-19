#pragma once

#include "../types.h"

#include <vector>

namespace probabilistic {
namespace pdbs {
namespace multiplicativity {

template <bool ignore_deterministic=false>
std::vector<std::vector<int>>
build_compatibility_graph_orthogonality(const PatternCollection& patterns);

std::vector<std::vector<int>>
build_compatibility_graph_weak_orthogonality(const PatternCollection& patterns);

}
}
}
