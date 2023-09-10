#ifndef PROBFD_HEURISTICS_PDBS_MAX_ORTHOGONALITY_COMBINATOR_H
#define PROBFD_HEURISTICS_PDBS_MAX_ORTHOGONALITY_COMBINATOR_H

#include "probfd/heuristics/pdbs/subcollection_combinator.h"

#include "probfd/task_proxy.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class MaxOrthogonalityCombinatorBase : public SubCollectionCombinator {
    const std::vector<std::vector<bool>> var_orthogonality;

public:
    explicit MaxOrthogonalityCombinatorBase(ProbabilisticTaskProxy task_proxy);

    void update(const PPDBCollection&) override;

    std::vector<PatternSubCollection> update_with_pdbs(
        const PPDBCollection& pdbs,
        const ProbabilityAwarePatternDatabase& new_pdb) override;
};

class AdditiveMaxOrthogonalityCombinator
    : public MaxOrthogonalityCombinatorBase {
public:
    using MaxOrthogonalityCombinatorBase::MaxOrthogonalityCombinatorBase;

    value_t evaluate_subcollection(
        const std::vector<value_t>& pdb_estimates,
        const std::vector<int>& subcollection) const override;

    value_t combine(value_t left, value_t right) const override;
};

class MultiplicativeMaxOrthogonalityCombinator
    : public MaxOrthogonalityCombinatorBase {
public:
    using MaxOrthogonalityCombinatorBase::MaxOrthogonalityCombinatorBase;

    value_t evaluate_subcollection(
        const std::vector<value_t>& pdb_estimates,
        const std::vector<int>& subcollection) const override;

    value_t combine(value_t left, value_t right) const override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_MAX_ORTHOGONALITY_COMBINATOR_H
