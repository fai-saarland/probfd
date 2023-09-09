#ifndef PROBFD_HEURISTICS_PDBS_MAX_ORTHOGONAL_FINDER_H
#define PROBFD_HEURISTICS_PDBS_MAX_ORTHOGONAL_FINDER_H

#include "probfd/heuristics/pdbs/subcollection_finder.h"

#include "probfd/task_proxy.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class MaxOrthogonalityFinderBase : public SubCollectionFinder {
    const std::vector<std::vector<bool>> var_orthogonality;

public:
    explicit MaxOrthogonalityFinderBase(ProbabilisticTaskProxy task_proxy);

    std::vector<PatternSubCollection>
    compute_subcollections(const PPDBCollection&) override;

    std::vector<PatternSubCollection> compute_subcollections_with_pdbs(
        const PPDBCollection& pdbs,
        const std::vector<PatternSubCollection>& known_pattern_cliques,
        const ProbabilityAwarePatternDatabase& new_pdb) override;
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