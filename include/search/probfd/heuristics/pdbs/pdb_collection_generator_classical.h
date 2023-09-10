#ifndef PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_GENERATOR_CLASSICAL_H
#define PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_GENERATOR_CLASSICAL_H

#include "probfd/heuristics/pdbs/pdb_collection_generator.h"

#include "downward/pdbs/pattern_generator.h"

#include <memory>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
namespace heuristics {
namespace pdbs {

class PDBCombinatorFactory;

/**
 * @brief The classical pattern generator adapter.
 *
 * Uses a classical pattern generation method on the determinization of the
 * input task. If classical PDBs are constructed by the generation algorithm,
 * they are used as a heuristic to compute the corresponding probability-aware
 * PDBs.
 */
class PDBCollectionGeneratorClassical : public PDBCollectionGenerator {
    std::shared_ptr<::pdbs::PatternCollectionGenerator> gen;
    std::shared_ptr<PDBCombinatorFactory> pdb_combinator_factory;

    PDBCollectionGeneratorClassical(
        const utils::LogProxy& log,
        std::shared_ptr<::pdbs::PatternCollectionGenerator> gen,
        std::shared_ptr<PDBCombinatorFactory> pdb_combinator_factory);

public:
    PDBCollectionGeneratorClassical(const plugins::Options& opts);
    ~PDBCollectionGeneratorClassical() override = default;

    PDBCollectionInformation generate(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_GENERATOR_CLASSICAL_H
