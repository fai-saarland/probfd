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

extern void run_cegar_loop(
    SingleCEGARResult& result,
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    cegar::FlawFindingStrategy& flaw_strategy,
    std::unordered_set<int> blacklisted_variables,
    int max_pdb_size,
    utils::RandomNumberGenerator& rng,
    bool wildcard,
    double max_time,
    utils::LogProxy log);

extern void add_cegar_wildcard_option_to_feature(plugins::Feature& feature);

} // namespace probfd::pdbs::cegar

#endif // PROBFD_PDBS_CEGAR_SINGLE_CEGAR_H
