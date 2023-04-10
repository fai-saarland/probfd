#ifndef PROBFD_HEURISTICS_PDBS_MAX_ORTHOGONAL_FINDER_H
#define PROBFD_HEURISTICS_PDBS_MAX_ORTHOGONAL_FINDER_H

#include "probfd/heuristics/pdbs/subcollection_finder.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class MaxOrthogonalityFinderBase : public SubCollectionFinder {
    const std::vector<std::vector<bool>> var_orthogonality;

public:
    explicit MaxOrthogonalityFinderBase(
        const ProbabilisticTaskProxy& task_proxy);

    std::shared_ptr<std::vector<PatternSubCollection>>
    compute_subcollections(const PatternCollection&) override;

    std::vector<PatternSubCollection> compute_subcollections_with_pattern(
        const PatternCollection& patterns,
        const std::vector<PatternSubCollection>& known_pattern_cliques,
        const Pattern& new_pattern) override;
};

class AdditiveMaxOrthogonalityFinder : public MaxOrthogonalityFinderBase {
public:
    using MaxOrthogonalityFinderBase::MaxOrthogonalityFinderBase;

    value_t evaluate_subcollection(
        const std::vector<value_t>& pdb_estimates,
        const std::vector<int>& subcollection) const override;

    value_t combine(value_t left, value_t right) const override;
};

class MultiplicativeMaxOrthogonalityFinder : public MaxOrthogonalityFinderBase {
public:
    using MaxOrthogonalityFinderBase::MaxOrthogonalityFinderBase;

    value_t evaluate_subcollection(
        const std::vector<value_t>& pdb_estimates,
        const std::vector<int>& subcollection) const override;

    value_t combine(value_t left, value_t right) const override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __MAX_ORTHOGONAL_FINDER_H__