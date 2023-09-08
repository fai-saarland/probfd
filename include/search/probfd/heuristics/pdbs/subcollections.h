#ifndef PROBFD_HEURISTICS_PDBS_ORTHOGONALITY_H
#define PROBFD_HEURISTICS_PDBS_ORTHOGONALITY_H

#include "probfd/heuristics/pdbs/types.h"

#include <vector>

namespace probfd {

class ProbabilisticOperator;
class ProbabilisticTaskProxy;

namespace heuristics {
namespace pdbs {

std::vector<std::vector<bool>> compute_prob_orthogonal_vars(
    ProbabilisticTaskProxy task_proxy,
    bool ignore_deterministic = false);

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    ProbabilisticTaskProxy task_proxy,
    const PatternCollection& patterns,
    bool ignore_deterministic = false);

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    const PatternCollection& patterns,
    const std::vector<std::vector<bool>>& var_orthogonality);

std::vector<std::vector<int>>
build_compatibility_graph_weak_orthogonality(const PatternCollection& patterns);

bool is_independent_collection(const PatternCollection& patterns);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __ORTHOGONALITY_H__