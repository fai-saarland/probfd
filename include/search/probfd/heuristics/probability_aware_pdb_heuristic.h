#ifndef PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H
#define PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H

#include "probfd/pdbs/types.h"

#include "probfd/heuristics/task_dependent_heuristic.h"

#include <memory>
#include <vector>

// Forward Declarations
namespace utils {
class LogProxy;
}

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
 * algorithms (see options).
 */
class ProbabilityAwarePDBHeuristic : public TaskDependentHeuristic {
    const value_t termination_cost_;

    std::shared_ptr<std::vector<pdbs::Pattern>> patterns_;
    std::shared_ptr<pdbs::PPDBCollection> pdbs_;
    std::shared_ptr<std::vector<pdbs::PatternSubCollection>> subcollections_;
    std::shared_ptr<pdbs::SubCollectionFinder> subcollection_finder_;

public:
    ProbabilityAwarePDBHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator,
        double max_time_dominance_pruning,
        utils::LogProxy log);

    value_t evaluate(const State& state) const override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H
