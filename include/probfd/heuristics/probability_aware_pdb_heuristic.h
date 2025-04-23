#ifndef PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H
#define PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H

#include "probfd/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"

#include "probfd/task_heuristic_factory.h"

#include "downward/utils/logging.h"

#include <memory>
#include <vector>

namespace probfd::pdbs {
class PatternCollectionGenerator;
class SubCollectionFinder;
} // namespace probfd::pdbs

namespace probfd::heuristics {

/**
 * @brief The probability-aware PDB heuristic.
 *
 * Combines multiple projections heuristics. The type of combination is
 * specified by the configuration of the pattern collection generation
 * algorithm.
 */
class ProbabilityAwarePDBHeuristic final : public FDREvaluator {
    const pdbs::PPDBCollection pdbs_;
    const std::vector<pdbs::PatternSubCollection> subcollections_;
    const std::shared_ptr<pdbs::SubCollectionFinder> subcollection_finder_;

    const value_t termination_cost_;

public:
    ProbabilityAwarePDBHeuristic(
        pdbs::PPDBCollection pdbs,
        std::vector<pdbs::PatternSubCollection> subcollections,
        std::shared_ptr<pdbs::SubCollectionFinder> subcollection_finder,
        value_t termination_cost);

    value_t evaluate(const downward::State& state) const override;
};

class ProbabilityAwarePDBHeuristicFactory final : public TaskHeuristicFactory {
    const std::shared_ptr<pdbs::PatternCollectionGenerator> generator_;
    const double max_time_dominance_pruning_;
    mutable downward::utils::LogProxy log_;

public:
    ProbabilityAwarePDBHeuristicFactory(
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator,
        double max_time_dominance_pruning,
        downward::utils::Verbosity verbosity);

    std::unique_ptr<FDREvaluator>
    create_heuristic(std::shared_ptr<ProbabilisticTask> task) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H
