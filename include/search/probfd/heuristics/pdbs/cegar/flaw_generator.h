#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_GENERATOR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_GENERATOR_H

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

namespace utils {
class CountdownTimer;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProjectionInfo;

namespace cegar {

struct Flaw;
class PolicyExplorationStrategy;

class FlawGenerator {
    // behavior defining parameters
    const std::shared_ptr<PolicyExplorationStrategy> exploration_strategy;
    const bool wildcard;
    const int max_pdb_size;

    std::unordered_set<int> blacklisted_variables;

public:
    FlawGenerator(
        std::shared_ptr<PolicyExplorationStrategy> exploration_strategy,
        bool wildcard,
        int max_pdb_size,
        std::unordered_set<int> blacklisted_variables);

    // Returns whether the flaw check was exhaustive.
    bool generate_flaws(
        ProbabilisticTaskProxy task_proxy,
        ProjectionInfo& info,
        utils::RandomNumberGenerator& rng,
        std::vector<Flaw>& flaws,
        utils::LogProxy log,
        utils::CountdownTimer& timer);
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_GENERATOR_H
