#ifndef MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_MAX_ORTHOGONAL_FINDER_H
#define MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_MAX_ORTHOGONAL_FINDER_H

#include "probfd/heuristics/pdbs/subcollections/subcollection_finder.h"

#include "probfd/heuristics/pdbs/types.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class MaxOrthogonalityFinder : public SubCollectionFinder {
    const VariableOrthogonality var_orthogonality;

public:
    explicit MaxOrthogonalityFinder(const ProbabilisticTaskProxy& task_proxy);

    std::shared_ptr<std::vector<PatternSubCollection>>
    compute_subcollections(const PatternCollection&) override;

    virtual std::vector<PatternSubCollection>
    compute_subcollections_with_pattern(
        const PatternCollection& patterns,
        const std::vector<PatternSubCollection>& known_pattern_cliques,
        const Pattern& new_pattern) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __MAX_ORTHOGONAL_FINDER_H__