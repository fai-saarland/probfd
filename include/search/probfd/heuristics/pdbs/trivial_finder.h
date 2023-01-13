#ifndef PROBFD_HEURISTICS_PDBS_TRIVIAL_FINDER_H
#define PROBFD_HEURISTICS_PDBS_TRIVIAL_FINDER_H

#include "probfd/heuristics/pdbs/subcollection_finder.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class TrivialFinder : public SubCollectionFinder {
public:
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

#endif // __TRIVIAL_FINDER_H__