#ifndef PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_CLASSICAL_H
#define PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_CLASSICAL_H

#include "probfd/pdbs/pattern_collection_generator.h"

#include <memory>

namespace downward::pdbs {
class PatternCollectionGenerator;
}

// Forward Declarations
namespace probfd::pdbs {
class SubCollectionFinderFactory;
}

namespace probfd::pdbs {

/**
 * @brief The classical pattern generator adapter.
 *
 * Uses a classical pattern generation method on the determinization of the
 * input task. If classical PDBs are constructed by the generation algorithm,
 * they are used as a heuristic to compute the corresponding probability-aware
 * PDBs.
 */
class PatternCollectionGeneratorClassical : public PatternCollectionGenerator {
    std::shared_ptr<downward::pdbs::PatternCollectionGenerator> gen_;
    std::shared_ptr<SubCollectionFinderFactory> finder_factory_;

public:
    explicit PatternCollectionGeneratorClassical(
        std::shared_ptr<downward::pdbs::PatternCollectionGenerator> gen,
        std::shared_ptr<SubCollectionFinderFactory> finder_factory,
        downward::utils::Verbosity verbosity);

    PatternCollectionInformation
    generate(const SharedProbabilisticTask& task) override;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_CLASSICAL_H
