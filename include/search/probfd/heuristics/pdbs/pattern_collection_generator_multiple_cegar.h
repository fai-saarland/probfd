#ifndef PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H

#include "probfd/heuristics/pdbs/pattern_collection_generator_multiple.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

class SubCollectionFinderFactory;

namespace cegar {
class FlawFindingStrategy;
}

class PatternCollectionGeneratorMultipleCegar
    : public PatternCollectionGeneratorMultiple {
    const bool use_wildcard_plans;
    std::shared_ptr<SubCollectionFinderFactory> subcollection_finder_factory;
    std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy;

    PatternInformation compute_pattern(
        int max_pdb_size,
        double max_time,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const ProbabilisticTaskProxy& task_proxy,
        TaskCostFunction& task_cost_function,
        const FactPair& goal,
        std::unordered_set<int>&& blacklisted_variables) override;

public:
    explicit PatternCollectionGeneratorMultipleCegar(options::Options& opts);
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif
