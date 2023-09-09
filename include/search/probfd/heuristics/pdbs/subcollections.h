#ifndef PROBFD_HEURISTICS_PDBS_ORTHOGONALITY_H
#define PROBFD_HEURISTICS_PDBS_ORTHOGONALITY_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_proxy.h"

#include "downward/pdbs/pattern_cliques.h"

#include <ranges>
#include <vector>

namespace probfd {
namespace heuristics {
namespace pdbs {

std::vector<std::vector<bool>> compute_prob_orthogonal_vars(
    ProbabilisticTaskProxy task_proxy,
    bool ignore_deterministic = false);

std::vector<std::vector<int>> build_compatibility_graph_orthogonality(
    const std::ranges::random_access_range auto& patterns,
    const std::vector<std::vector<bool>>& on_vars)
{
    using ::pdbs::are_patterns_additive;

    std::vector<std::vector<int>> cgraph;
    cgraph.resize(patterns.size());

    for (size_t i = 0; i < patterns.size(); ++i) {
        for (size_t j = i + 1; j < patterns.size(); ++j) {
            if (are_patterns_additive(patterns[i], patterns[j], on_vars)) {
                /* If the two patterns are additive, there is an edge in the
                   compatibility graph. */
                cgraph[i].push_back(j);
                cgraph[j].push_back(i);
            }
        }
    }

    return cgraph;
}

std::vector<std::vector<int>>
build_compatibility_graph_weak_orthogonality(const PatternCollection& patterns);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __ORTHOGONALITY_H__