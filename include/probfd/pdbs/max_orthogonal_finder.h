#ifndef PROBFD_PDBS_MAX_ORTHOGONAL_FINDER_H
#define PROBFD_PDBS_MAX_ORTHOGONAL_FINDER_H

#include "probfd/pdbs/subcollection_finder.h"

#include <memory>
#include <vector>

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::pdbs {

class MaxOrthogonalityFinderBase : public SubCollectionFinder {
    const std::vector<std::vector<bool>> var_orthogonality_;

public:
    explicit MaxOrthogonalityFinderBase(
        const ProbabilisticTaskProxy& task_proxy);

    std::vector<PatternSubCollection>
    compute_subcollections(const PatternCollection&) override;

    std::vector<PatternSubCollection> compute_subcollections_with_pattern(
        const PatternCollection& patterns,
        const std::vector<PatternSubCollection>& known_pattern_cliques,
        const Pattern& new_pattern) override;
};

class AdditiveMaxOrthogonalityFinder : public MaxOrthogonalityFinderBase {
public:
    using MaxOrthogonalityFinderBase::MaxOrthogonalityFinderBase;

    [[nodiscard]]
    value_t evaluate_subcollection(
        const std::vector<value_t>& pdb_estimates,
        const std::vector<int>& subcollection) const override;

    [[nodiscard]]
    value_t combine(value_t left, value_t right) const override;
};

class MultiplicativeMaxOrthogonalityFinder : public MaxOrthogonalityFinderBase {
public:
    using MaxOrthogonalityFinderBase::MaxOrthogonalityFinderBase;

    [[nodiscard]]
    value_t evaluate_subcollection(
        const std::vector<value_t>& pdb_estimates,
        const std::vector<int>& subcollection) const override;

    [[nodiscard]]
    value_t combine(value_t left, value_t right) const override;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_MAX_ORTHOGONAL_FINDER_H
