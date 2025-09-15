#ifndef PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H
#define PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H

#include "probfd/pdbs/pattern_collection_generator.h"

#include "probfd/aliases.h"

#include <memory>

// Forward Declarations
namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd::pdbs {
class SubCollectionFinderFactory;
}

namespace probfd::pdbs::cegar {
class FlawFindingStrategy;
} // namespace probfd::pdbs::cegar

namespace probfd::pdbs {

class PatternCollectionGeneratorDisjointCegar
    : public PatternCollectionGenerator {
    const value_t convergence_epsilon_;
    const bool use_wildcard_policies_;
    const bool single_goal_;
    const int max_pdb_size_;
    const int max_collection_size_;
    const downward::utils::Duration max_time_;
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;
    std::shared_ptr<SubCollectionFinderFactory> subcollection_finder_factory_;
    std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy_;

public:
    explicit PatternCollectionGeneratorDisjointCegar(
        value_t convergence_epsilon,
        bool use_wildcard_policies,
        bool single_goal,
        int max_pdb_size,
        int max_collection_size,
        downward::utils::Duration max_time,
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng,
        const std::shared_ptr<SubCollectionFinderFactory>&
            subcollection_finder_factory,
        const std::shared_ptr<cegar::FlawFindingStrategy>& flaw_strategy,
        downward::utils::Verbosity verbosity);

    PatternCollectionInformation generate(
        const SharedProbabilisticTask& task) override;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H
