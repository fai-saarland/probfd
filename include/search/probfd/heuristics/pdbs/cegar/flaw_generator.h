#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_GENERATOR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_GENERATOR_H

#include "probfd/heuristics/pdbs/cegar/flaw.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <optional>

namespace utils {
class CountdownTimer;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProjectionInfo;

namespace cegar {

class PolicyExplorationStrategy;

class FlawGenerator {
    const std::shared_ptr<PolicyExplorationStrategy> exploration_strategy;
    const std::shared_ptr<utils::RandomNumberGenerator> rng;
    const bool wildcard;
    const int max_pdb_size;

    std::unordered_set<int> blacklisted_variables;

    std::vector<Flaw> flaws;

public:
    FlawGenerator(
        std::shared_ptr<PolicyExplorationStrategy> exploration_strategy,
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        bool wildcard,
        int max_pdb_size,
        std::unordered_set<int> blacklisted_variables);

    // If no flaw could be found, returns std::nullopt.
    std::optional<Flaw> next_flaw(
        ProbabilisticTaskProxy task_proxy,
        ProjectionInfo& info,
        utils::LogProxy log,
        utils::CountdownTimer& timer);
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_CEGAR_FLAW_GENERATOR_H
