#ifndef PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
#define PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H

#include "probfd/heuristics/pdbs/pattern_collection_generator.h"

#include "probfd/engine_interfaces/types.h"

#include <memory>
#include <set>
#include <unordered_set>
#include <utility>

namespace plugins {
class Feature;
}

namespace utils {
class CountdownTimer;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace pdbs {

class PatternCollectionGeneratorMultiple : public PatternCollectionGenerator {
    const std::string implementation_name;
    const int max_pdb_size;
    const int max_collection_size;
    const double pattern_generation_max_time;
    const double total_max_time;
    const double stagnation_limit;
    const double blacklisting_start_time;
    const bool enable_blacklist_on_stagnation;
    const std::shared_ptr<utils::RandomNumberGenerator> rng;
    const int random_seed;
    const bool use_saturated_costs;

    bool collection_size_limit_reached(int remaining_collection_size) const;
    bool time_limit_reached(const utils::CountdownTimer& timer) const;

    virtual std::pair<
        std::shared_ptr<ProjectionStateSpace>,
        std::shared_ptr<ProbabilityAwarePatternDatabase>>
    compute_pattern(
        int max_pdb_size,
        double max_time,
        const std::shared_ptr<utils::RandomNumberGenerator>& rng,
        const ProbabilisticTaskProxy& task_proxy,
        TaskSimpleCostFunction& task_cost_function,
        const FactPair& goal,
        std::unordered_set<int>&& blacklisted_variables) = 0;

    virtual PatternCollectionInformation
    generate(const std::shared_ptr<ProbabilisticTask>& task) override;

public:
    explicit PatternCollectionGeneratorMultiple(
        const plugins::Options& opts,
        std::string implementation_name);
};

extern void add_multiple_options_to_feature(plugins::Feature& feature);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif
