#ifndef PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H
#define PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_CEGAR_H

#include "downward/pdbs/pattern_collection_generator_multiple.h"

namespace downward::pdbs {
class PatternCollectionGeneratorMultipleCegar
    : public PatternCollectionGeneratorMultiple {
    const bool use_wildcard_plans;

    std::string id() const override;
    void initialize(const SharedAbstractTask&) override {}
    PatternInformation compute_pattern(
        int max_pdb_size,
        utils::Duration max_time,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const SharedAbstractTask& task,
        const FactPair& goal,
        std::unordered_set<int>&& blacklisted_variables) override;

public:
    PatternCollectionGeneratorMultipleCegar(
        bool use_wildcard_plans,
        int max_pdb_size,
        int max_collection_size,
        utils::Duration pattern_generation_max_time,
        utils::Duration total_max_time,
        utils::Duration stagnation_limit,
        double blacklist_trigger_percentage,
        bool enable_blacklist_on_stagnation,
        int random_seed,
        utils::Verbosity verbosity);
};
} // namespace pdbs

#endif
