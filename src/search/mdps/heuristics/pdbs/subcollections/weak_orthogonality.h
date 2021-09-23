#ifndef MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_WEAK_ORTHOGONALITY_H
#define MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_WEAK_ORTHOGONALITY_H

#include "../types.h"

#include <vector>

namespace probabilistic {
namespace pdbs {

std::vector<std::vector<int>>
build_compatibility_graph_weak_orthogonality(const PatternCollection& patterns);

} // namespace pdbs
} // namespace probabilistic

#endif // __WEAK_ORTHOGONALITY_H__