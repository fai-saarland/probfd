#ifndef PROBFD_PDBS_CEGAR_FLAW_FINDING_STRATEGY_H
#define PROBFD_PDBS_CEGAR_FLAW_FINDING_STRATEGY_H

#include "probfd/pdbs/types.h"

#include <string>
#include <unordered_set>
#include <vector>

// Forward Declarations
class State;
class PreconditionsProxy;
class GoalsProxy;

namespace utils {
class CountdownTimer;
}

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::pdbs {
class StateRankingFunction;
}

namespace probfd::pdbs::cegar {
struct Flaw;
}

namespace probfd::pdbs::cegar {

class FlawFindingStrategy {
public:
    virtual ~FlawFindingStrategy() = default;

    // Returns whether policy is executable (modulo blacklisting)
    // Note that the output flaw list might be empty regardless since only
    // remaining goals are added to the list for goal violations.
    virtual bool apply_policy(
        const ProbabilisticTaskProxy& task_proxy,
        const StateRankingFunction& state_ranking_function,
        const ProjectionStateSpace& mdp,
        const ProjectionMultiPolicy& policy,
        const std::unordered_set<int>& blacklisted_variables,
        std::vector<Flaw>& flaws,
        utils::CountdownTimer& timer) = 0;

    virtual std::string get_name() = 0;
};

bool collect_flaws(
    PreconditionsProxy facts,
    const State& state,
    const std::unordered_set<int>& blacklist,
    std::vector<Flaw>& flaw_list);

bool collect_flaws(
    GoalsProxy facts,
    const State& state,
    const std::unordered_set<int>& blacklist,
    std::vector<Flaw>& flaw_list);

} // namespace probfd::pdbs::cegar

#endif // PROBFD_PDBS_CEGAR_FLAW_FINDING_STRATEGY_H
