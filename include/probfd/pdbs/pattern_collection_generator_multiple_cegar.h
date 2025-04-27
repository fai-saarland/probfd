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
    const value_t convergence_epsilon_;
    std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy_;
    const bool use_wildcard_policies_;

    ProjectionTransformation compute_pattern(
        int max_pdb_size,
        double max_time,
        const std::shared_ptr<downward::utils::RandomNumberGenerator>& rng,
        const SharedProbabilisticTask& task,
        const downward::FactPair& goal,
        std::unordered_set<int>&& blacklisted_variables) override;

public:
    explicit PatternCollectionGeneratorMultipleCegar(
        value_t convergence_epsilon,
        std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy,
        bool use_wildcard_policies,
        int max_pdb_size,
        int max_collection_size,
        double pattern_generation_max_time,
        double total_max_time,
        double stagnation_limit,
        double blacklist_trigger_percentage,
        bool enable_blacklist_on_stagnation,
        bool use_saturated_costs,
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng,
        downward::utils::Verbosity verbosity);
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H
