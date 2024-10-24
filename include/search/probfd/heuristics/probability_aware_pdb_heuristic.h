#ifndef PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H
#define PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H

#include "probfd/pdbs/types.h"

#include "probfd/heuristics/task_dependent_heuristic.h"
#include "probfd/task_evaluator_factory.h"

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
 * algorithm.
 */
class ProbabilityAwarePDBHeuristic : public TaskDependentHeuristic {
    const value_t termination_cost_;

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

class ProbabilityAwarePDBHeuristicFactory : public TaskEvaluatorFactory {
    const std::shared_ptr<probfd::pdbs::PatternCollectionGenerator> patterns_;
    const double max_time_dominance_pruning_;
    const utils::Verbosity verbosity_;

public:
    ProbabilityAwarePDBHeuristicFactory(
        std::shared_ptr<probfd::pdbs::PatternCollectionGenerator> patterns,
        double max_time_dominance_pruning,
        utils::Verbosity verbosity);

    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H
