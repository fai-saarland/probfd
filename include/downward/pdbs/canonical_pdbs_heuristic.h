#ifndef PDBS_CANONICAL_PDBS_HEURISTIC_H
#define PDBS_CANONICAL_PDBS_HEURISTIC_H

#include "downward/pdbs/canonical_pdbs.h"
#include "downward/pdbs/pattern_generator.h"

#include "downward/heuristic.h"

namespace downward::pdbs {

// Implements the canonical heuristic function.
class CanonicalPDBsHeuristic : public Heuristic {
    CanonicalPDBs canonical_pdbs;

protected:
    int compute_heuristic(const State& ancestor_state) override;

public:
    CanonicalPDBsHeuristic(
        const std::shared_ptr<PatternCollectionGenerator>& patterns,
        utils::Duration max_time_dominance_pruning,
        SharedAbstractTask original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    CanonicalPDBsHeuristic(
        const std::shared_ptr<PatternCollectionGenerator>& patterns,
        utils::Duration max_time_dominance_pruning,
        SharedAbstractTask original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);
};

} // namespace pdbs

#endif
