#ifndef PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_GENERATOR_DISJOINT_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_GENERATOR_DISJOINT_CEGAR_H

#include "probfd/heuristics/pdbs/pdb_collection_generator.h"
#include "probfd/heuristics/pdbs/types.h"

#include "downward/plugins/options.h"

#include "downward/utils/rng.h"

namespace plugins {
class Feature;
class Options;
} // namespace plugins

namespace probfd {
namespace heuristics {
namespace pdbs {

class PDBCombinatorFactory;

namespace cegar {
class PolicyExplorationStrategy;
} // namespace cegar

class PDBCollectionGeneratorDisjointCegar : public PDBCollectionGenerator {
    const bool use_wildcard_policies;
    const bool single_goal;
    const int max_pdb_size;
    const int max_collection_size;
    const double max_time;
    std::shared_ptr<utils::RandomNumberGenerator> rng;
    std::shared_ptr<PDBCombinatorFactory> pdb_combinator_factory;
    std::shared_ptr<cegar::PolicyExplorationStrategy> exploration_strategy;

public:
    explicit PDBCollectionGeneratorDisjointCegar(const plugins::Options& opts);

    virtual ~PDBCollectionGeneratorDisjointCegar() = default;

    PDBCollectionInformation generate(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override;
};

void add_pdb_collection_generator_cegar_options_to_feature(
    plugins::Feature& feature);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_GENERATOR_DISJOINT_CEGAR_H
