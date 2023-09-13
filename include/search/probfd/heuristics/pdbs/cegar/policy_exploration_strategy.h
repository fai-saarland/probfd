#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_POLICY_EXPLORATION_STRATEGY_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_POLICY_EXPLORATION_STRATEGY_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/heuristics/pdbs/cegar/flaw.h"

#include "probfd/task_proxy.h"

#include <string>
#include <unordered_set>

namespace utils {
class CountdownTimer;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProbabilityAwarePatternDatabase;
class ProjectionStateSpace;
struct ProjectionInfo;

namespace cegar {

struct Flaw;

using FlawFilter = std::function<bool(int)>;

class PolicyExplorationStrategy {
public:
    virtual ~PolicyExplorationStrategy() = default;

    // Returns whether the policy is executable. Note that even if the policy is
    // not executable, the output flaw list might be empty regardless, since
    // blacklisted variables are never added.
    virtual bool apply_policy(
        ProbabilisticTaskProxy task_proxy,
        const ProjectionInfo& info,
        const ProjectionMultiPolicy& policy,
        FlawFilter& flaw_filter,
        std::vector<Flaw>& flaws,
        utils::CountdownTimer& timer) = 0;

    virtual std::string get_name() = 0;
};

bool collect_flaws(
    PreconditionsProxy facts,
    const State& state,
    FlawFilter& flaw_filter,
    std::vector<Flaw>& flaw_list);

bool collect_flaws(
    GoalsProxy facts,
    const State& state,
    FlawFilter& flaw_filter,
    std::vector<Flaw>& flaw_list);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_CEGAR_POLICY_EXPLORATION_STRATEGY_H
