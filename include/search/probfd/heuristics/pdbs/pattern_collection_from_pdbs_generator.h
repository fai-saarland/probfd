#ifndef PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_FROM_PDBS_GENERATOR_H
#define PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_FROM_PDBS_GENERATOR_H

#include "probfd/heuristics/pdbs/pattern_collection_generator.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace pdbs {

class PDBCollectionGenerator;

class PatternCollectionFromPDBsGenerator : public PatternCollectionGenerator {
    std::shared_ptr<PDBCollectionGenerator> pdb_collection_generator;

public:
    explicit PatternCollectionFromPDBsGenerator(const plugins::Options& opts);
    PatternCollectionFromPDBsGenerator(
        utils::LogProxy log,
        std::shared_ptr<PDBCollectionGenerator> pdb_collection_generator);

    std::shared_ptr<PatternCollection> generate_pattern_collection(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_FROM_PDBS_GENERATOR_H
