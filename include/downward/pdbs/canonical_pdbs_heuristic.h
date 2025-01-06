#ifndef PDBS_CANONICAL_PDBS_HEURISTIC_H
#define PDBS_CANONICAL_PDBS_HEURISTIC_H

#include "downward/pdbs/canonical_pdbs.h"
#include "downward/pdbs/pattern_generator.h"

#include "downward/heuristic.h"

namespace pdbs {

// Implements the canonical heuristic function.
class CanonicalPDBsHeuristic : public Heuristic {
    CanonicalPDBs canonical_pdbs;

protected:
    virtual int compute_heuristic(const State& ancestor_state) override;

public:
    CanonicalPDBsHeuristic(
        const std::shared_ptr<PatternCollectionGenerator>& patterns,
        double max_time_dominance_pruning,
        const std::shared_ptr<AbstractTask>& transform,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);
};

} // namespace pdbs

#endif
