#ifndef PROBFD_HEURISTICS_PDBS_PROBAIBLITY_AWARE_PDB_HEURISTIC_H
#define PROBFD_HEURISTICS_PDBS_PROBAIBLITY_AWARE_PDB_HEURISTIC_H

#include "probfd/heuristics/pdbs/pdb_collection_generator.h"

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/heuristics/task_dependent_heuristic.h"

#include <memory>
#include <ostream>
#include <vector>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
namespace heuristics {
namespace pdbs {

class PDBCombinator;

/**
 * @brief The probability-aware PDB heuristic.
 *
 * Combines multiple projections heuristics. The type of combination is
 * specified by the configuration of the pattern collection generation
 * algorithms (see options).
 */
class ProbabilityAwarePDBHeuristic : public TaskDependentHeuristic {
    const value_t termination_cost;

    PPDBCollection pdbs;
    std::shared_ptr<PDBCombinator> pdb_combinator;

public:
    ProbabilityAwarePDBHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::shared_ptr<PDBCollectionGenerator> generator,
        double max_time_dominance_pruning,
        utils::LogProxy log);

    value_t evaluate(const State& state) const override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __PROBABILISTIC_PDB_HEURISTIC_H__