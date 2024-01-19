#ifndef PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H
#define PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H

#include "probfd/pdbs/pattern_collection_generator_multiple.h"

#include <memory>
#include <unordered_set>
#include <utility>

namespace probfd::pdbs::cegar {
class FlawFindingStrategy;
}

namespace probfd::pdbs {

class PatternCollectionGeneratorMultipleCegar
    : public PatternCollectionGeneratorMultiple {
    const bool use_wildcard_policies_;
    std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy_;

    std::pair<
        std::shared_ptr<ProjectionStateSpace>,
        std::shared_ptr<ProbabilityAwarePatternDatabase>>
    compute_pattern(
        int max_pdb_size,
        double max_time,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const ProbabilisticTaskProxy& task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const FactPair& goal,
        std::unordered_set<int>&& blacklisted_variables) override;

public:
    explicit PatternCollectionGeneratorMultipleCegar(
        const plugins::Options& opts);
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H
