#ifndef PROBFD_PDBS_CEGAR_SINGLE_CEGAR_H
#define PROBFD_PDBS_CEGAR_SINGLE_CEGAR_H

#include "probfd/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/value_type.h"

#include <memory>
#include <unordered_set>
#include <vector>

// Forward Declarations
class VariablesProxy;

namespace plugins {
class Feature;
}

namespace utils {
class CountdownTimer;
class LogProxy;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::pdbs {
class ProjectionStateSpace;
class ProbabilityAwarePatternDatabase;
} // namespace probfd::pdbs

namespace probfd::pdbs::cegar {
struct Flaw;
class FlawFindingStrategy;
} // namespace probfd::pdbs::cegar

namespace probfd::pdbs::cegar {

struct SingleCEGARResult {
    std::unique_ptr<ProjectionStateSpace> projection;
    std::unique_ptr<ProbabilityAwarePatternDatabase> pdb;

    SingleCEGARResult(
        std::unique_ptr<ProjectionStateSpace>&& projection,
        std::unique_ptr<ProbabilityAwarePatternDatabase>&& pdb);
    SingleCEGARResult(SingleCEGARResult&&) noexcept;
    SingleCEGARResult& operator=(SingleCEGARResult&&) noexcept;
    ~SingleCEGARResult();
};

class SingleCEGAR {
    // Random number generator
    const std::shared_ptr<utils::RandomNumberGenerator> rng_;

    // Flaw finding strategy
    const std::shared_ptr<FlawFindingStrategy> flaw_strategy_;

    // behavior defining parameters
    const bool wildcard_;
    const int max_pdb_size_;

    const int var;
    std::unordered_set<int> blacklisted_variables_;

public:
    SingleCEGAR(
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy,
        bool wildcard,
        int max_pdb_size,
        int var,
        std::unordered_set<int> blacklisted_variables = {});

    ~SingleCEGAR();

    SingleCEGARResult generate_pdbs(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        double max_time,
        utils::LogProxy log);

private:
    bool get_flaws(
        SingleCEGARResult& result,
        ProbabilisticTaskProxy task_proxy,
        std::vector<Flaw>& flaws,
        const State& initial_state,
        utils::CountdownTimer& timer,
        utils::LogProxy log);

    void refine(
        SingleCEGARResult& result,
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const VariablesProxy& variables,
        const std::vector<Flaw>& flaws,
        utils::CountdownTimer& timer,
        utils::LogProxy log);
};

extern void add_cegar_wildcard_option_to_feature(plugins::Feature& feature);

} // namespace probfd::pdbs::cegar

#endif // PROBFD_PDBS_CEGAR_SINGLE_CEGAR_H
