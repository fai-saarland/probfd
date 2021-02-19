#pragma once

#include <vector>
#include <map>

#include "../../../globals.h"
#include "../../../probabilistic_operator.h"
#include "../syntactic_projection.h"
#include "../../../../pdbs/pattern_cliques.h"

namespace probabilistic {
namespace pdbs {
namespace multiplicativity {

bool is_independent_collection(const PatternCollection& patterns);

template <bool ignore_deterministic=false>
std::vector<std::vector<int>>
build_compatibility_graph_orthogonality(const PatternCollection& patterns);

std::vector<std::vector<int>>
build_compatibility_graph_weak_orthogonality(const PatternCollection& patterns);

}
}
}
