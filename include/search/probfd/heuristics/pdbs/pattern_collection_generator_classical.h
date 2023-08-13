#ifndef PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_CLASSICAL_H
#define PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_CLASSICAL_H

#include "probfd/heuristics/pdbs/pattern_collection_generator.h"

#include "downward/pdbs/pattern_generator.h"

#include <memory>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
namespace heuristics {
namespace pdbs {

class SubCollectionFinderFactory;

/**
 * @brief The classical pattern generator adapter.
 *
 * Uses a classical pattern generation method on the determinization of the
 * input task. If classical PDBs are constructed by the generation algorithm,
 * they are used as a heuristic to compute the corresponding probability-aware
 * PDBs.
 */
class PatternCollectionGeneratorClassical : public PatternCollectionGenerator {
    std::shared_ptr<::pdbs::PatternCollectionGenerator> gen;
    std::shared_ptr<SubCollectionFinderFactory> finder_factory;

    PatternCollectionGeneratorClassical(
        const utils::LogProxy& log,
        std::shared_ptr<::pdbs::PatternCollectionGenerator> gen,
        std::shared_ptr<SubCollectionFinderFactory> finder);

public:
    PatternCollectionGeneratorClassical(const plugins::Options& opts);
    ~PatternCollectionGeneratorClassical() override = default;

    PatternCollectionInformation generate(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<TaskCostFunction>& task_cost_function) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PATTERN_COLLECTION_GENERATOR_DETERMINISTIC_H_