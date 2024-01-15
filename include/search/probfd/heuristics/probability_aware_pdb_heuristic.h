#ifndef PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H
#define PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/types.h"

#include "probfd/heuristics/task_dependent_heuristic.h"

#include <memory>
#include <vector>

namespace utils {
class LogProxy;
}

namespace probfd {

namespace pdbs {
class SubCollectionFinder;
}

namespace heuristics {

/**
 * @brief The probability-aware PDB heuristic.
 *
 * Combines multiple projections heuristics. The type of combination is
 * specified by the configuration of the pattern collection generation
 * algorithms (see options).
 */
class ProbabilityAwarePDBHeuristic : public TaskDependentHeuristic {
    const value_t termination_cost;

    std::shared_ptr<std::vector<pdbs::Pattern>> patterns;
    std::shared_ptr<pdbs::PPDBCollection> pdbs;
    std::shared_ptr<std::vector<pdbs::PatternSubCollection>> subcollections;
    std::shared_ptr<pdbs::SubCollectionFinder> subcollection_finder;

public:
    ProbabilityAwarePDBHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator,
        double max_time_dominance_pruning,
        utils::LogProxy log);

    value_t evaluate(const State& state) const override;
};

} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H
