#ifndef MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_WEAK_ORTHOGONALITY_H
#define MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_WEAK_ORTHOGONALITY_H

#include "probfd/heuristics/pdbs/types.h"

#include <vector>

namespace probfd {
namespace heuristics {

namespace pdbs {

std::vector<std::vector<int>>
build_compatibility_graph_weak_orthogonality(const PatternCollection& patterns);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __WEAK_ORTHOGONALITY_H__