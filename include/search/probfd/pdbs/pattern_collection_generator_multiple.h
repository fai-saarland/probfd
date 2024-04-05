#ifndef PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
#define PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/projection_transformation.h"

#include "probfd/fdr_types.h"

#include <memory>
#include <string>
#include <unordered_set>
#include <utility>

// Forward Declarations
struct FactPair;

namespace plugins {
class Feature;
}

namespace utils {
class CountdownTimer;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::pdbs {
class ProjectionStateSpace;
class ProbabilityAwarePatternDatabase;
} // namespace probfd::pdbs

namespace probfd::pdbs {

class PatternCollectionGeneratorMultiple : public PatternCollectionGenerator {
    const std::string implementation_name_;
    const int max_pdb_size_;
    const int max_collection_size_;
    const double pattern_generation_max_time_;
    const double total_max_time_;
    const double stagnation_limit_;
    const double blacklisting_start_time_;
    const bool enable_blacklist_on_stagnation_;
    const std::shared_ptr<utils::RandomNumberGenerator> rng_;
    const int random_seed_;
    const bool use_saturated_costs_;

    bool collection_size_limit_reached(int remaining_collection_size) const;
    bool time_limit_reached(const utils::CountdownTimer& timer) const;

    virtual ProjectionTransformation compute_pattern(
        int max_pdb_size,
        double max_time,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const ProbabilisticTaskProxy& task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const FactPair& goal,
        std::unordered_set<int>&& blacklisted_variables) = 0;

    PatternCollectionInformation generate(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override;

public:
    explicit PatternCollectionGeneratorMultiple(
        const plugins::Options& opts,
        std::string implementation_name);
};

extern void add_multiple_options_to_feature(plugins::Feature& feature);

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
