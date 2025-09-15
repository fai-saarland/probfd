#ifndef PROBFD_PDBS_SUBCOLLECTIONS_H
#define PROBFD_PDBS_SUBCOLLECTIONS_H

#include "probfd/pdbs/types.h"

#include "probfd/probabilistic_task.h"

#include <vector>

namespace probfd::pdbs {

std::vector<std::vector<bool>> compute_prob_orthogonal_vars(
    const downward::VariableSpace& variables,
    const ProbabilisticOperatorSpace& operators,
    bool ignore_deterministic = false);

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    const downward::VariableSpace& variables,
    const ProbabilisticOperatorSpace& operators,
    const PatternCollection& patterns,
    bool ignore_deterministic = false);

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    const PatternCollection& patterns,
    const std::vector<std::vector<bool>>& var_orthogonality);

std::vector<std::vector<int>>
build_compatibility_graph_weak_orthogonality(const PatternCollection& patterns);

bool is_independent_collection(const PatternCollection& patterns);

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_SUBCOLLECTIONS_H
