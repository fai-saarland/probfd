#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_SINGLE_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_SINGLE_CEGAR_H

#include "probfd/heuristics/pdbs/evaluators.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/heuristics/pdbs/cegar/flaw.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <unordered_set>
#include <vector>

namespace utils {
class CountdownTimer;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProbabilityAwarePatternDatabase;
class ProjectionStateSpace;
struct ProjectionInfo;

namespace cegar {

class PolicyExplorationStrategy;

class SingleCEGAR {
    // Random number generator
    const std::shared_ptr<utils::RandomNumberGenerator> rng;

    // Flaw finding strategy
    const std::shared_ptr<PolicyExplorationStrategy> flaw_strategy;

    // behavior defining parameters
    const bool wildcard;
    const int max_pdb_size;

    std::unordered_set<int> blacklisted_variables;

public:
    SingleCEGAR(
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        std::shared_ptr<PolicyExplorationStrategy> flaw_strategy,
        bool wildcard,
        int max_pdb_size,
        std::unordered_set<int> blacklisted_variables = {});

    ~SingleCEGAR();

    void run_refinement_loop(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        ProjectionInfo& pdb_info,
        utils::LogProxy log,
        utils::CountdownTimer& timer);

private:
    // If a concrete solution was found, returns a pointer to the corresponding
    // info struct, otherwise nullptr.
    bool get_flaws(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        ProjectionInfo& pdb_info,
        std::vector<Flaw>& flaws,
        utils::LogProxy log,
        utils::CountdownTimer& timer);

    void refine(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        ProjectionInfo& pdb_info,
        const std::vector<Flaw>& flaws,
        utils::LogProxy log,
        utils::CountdownTimer& timer);
};

extern void add_cegar_wildcard_option_to_feature(plugins::Feature& feature);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_CEGAR_SINGLE_CEGAR_H
