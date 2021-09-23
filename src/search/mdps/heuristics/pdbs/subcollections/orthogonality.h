#ifndef MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_ORTHOGONALITY_H
#define MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_ORTHOGONALITY_H

#include "../types.h"

#include <vector>

namespace probabilistic {

class ProbabilisticOperator;

namespace pdbs {

std::vector<int>
get_affected_vars(const ProbabilisticOperator& op, bool ignore_deterministic);

VariableOrthogonality
compute_prob_orthogonal_vars(bool ignore_deterministic = false);

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    const PatternCollection& patterns,
    bool ignore_deterministic = false);

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    const PatternCollection& patterns,
    const VariableOrthogonality& var_orthogonality);

} // namespace pdbs
} // namespace probabilistic

#endif // __ORTHOGONALITY_H__