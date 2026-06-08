#ifndef PROBFD_HEURISTICS_PDB_MAX_HEURISTIC_H
#define PROBFD_HEURISTICS_PDB_MAX_HEURISTIC_H

#include "probfd/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"

#include "probfd/task_heuristic_factory.h"

#include "downward/utils/logging.h"

#include <memory>
#include <vector>

namespace probfd::pdbs {
class PatternCollectionGenerator;
} // namespace probfd::pdbs

namespace probfd::heuristics {

/**
 * @brief The probability-aware PDB heuristic.
 *
 * Combines multiple projections heuristics. The type of combination is
 * specified by the configuration of the pattern collection generation
 * algorithm.
 */
class PDBMaxHeuristic final : public FDRHeuristic {
    const pdbs::PPDBCollection pdbs_;

    const value_t termination_cost_;

public:
    PDBMaxHeuristic(pdbs::PPDBCollection pdbs, value_t termination_cost);

    value_t evaluate(const downward::State& state) const override;
};

class PDBMaxHeuristicFactory final : public TaskHeuristicFactory {
    const std::shared_ptr<pdbs::PatternCollectionGenerator> generator_;
    mutable downward::utils::LogProxy log_;

public:
    PDBMaxHeuristicFactory(
        std::shared_ptr<pdbs::PatternCollectionGenerator> generator,
        downward::utils::Verbosity verbosity);

    std::unique_ptr<FDRHeuristic>
    create_object(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_PROBABILITY_AWARE_PDB_HEURISTIC_H
