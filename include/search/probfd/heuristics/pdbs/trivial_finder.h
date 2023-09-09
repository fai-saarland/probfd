#ifndef PROBFD_HEURISTICS_PDBS_TRIVIAL_FINDER_H
#define PROBFD_HEURISTICS_PDBS_TRIVIAL_FINDER_H

#include "probfd/heuristics/pdbs/subcollection_finder.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class TrivialFinder : public SubCollectionFinder {
public:
    std::vector<PatternSubCollection>
    compute_subcollections(const PPDBCollection&) override;

    std::vector<PatternSubCollection> compute_subcollections_with_pdbs(
        const PPDBCollection& pdbs,
        const std::vector<PatternSubCollection>& known_pattern_cliques,
        const ProbabilityAwarePatternDatabase& new_pdb) override;

    value_t evaluate_subcollection(
        const std::vector<value_t>& pdb_estimates,
        const std::vector<int>& subcollection) const override;

    value_t combine(value_t left, value_t right) const override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TRIVIAL_FINDER_H__