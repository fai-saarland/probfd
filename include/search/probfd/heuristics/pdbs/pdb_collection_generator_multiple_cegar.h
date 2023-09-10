#ifndef PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H

#include "probfd/heuristics/pdbs/pdb_collection_generator_multiple.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

namespace cegar {
class PolicyExplorationStrategy;
}

class PDBCollectionGeneratorMultipleCegar
    : public PDBCollectionGeneratorMultiple {
    const bool use_wildcard_policies;
    std::shared_ptr<cegar::PolicyExplorationStrategy> exploration_strategy;

    ProjectionInfo compute_pattern(
        int max_pdb_size,
        double max_time,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const FactPair& goal,
        std::unordered_set<int>&& blacklisted_variables) override;

public:
    explicit PDBCollectionGeneratorMultipleCegar(const plugins::Options& opts);
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H
