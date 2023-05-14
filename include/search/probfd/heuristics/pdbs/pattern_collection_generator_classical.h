#ifndef PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_CLASSICAL_H
#define PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_CLASSICAL_H

#include "pdbs/pattern_generator.h"

#include "probfd/heuristics/pdbs/pattern_generator.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

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
    PatternCollectionGeneratorClassical(const options::Options& opts);
    ~PatternCollectionGeneratorClassical() override = default;

    PatternCollectionInformation
    generate(const std::shared_ptr<ProbabilisticTask>& task) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PATTERN_COLLECTION_GENERATOR_DETERMINISTIC_H_