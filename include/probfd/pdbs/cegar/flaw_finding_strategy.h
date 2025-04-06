#ifndef PROBFD_PDBS_CEGAR_FLAW_FINDING_STRATEGY_H
#define PROBFD_PDBS_CEGAR_FLAW_FINDING_STRATEGY_H

#include "probfd/pdbs/types.h"

#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

// Forward Declarations
namespace downward {
class State;
class PreconditionsProxy;
class GoalsProxy;
}

namespace downward::utils {
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
        std::vector<Flaw>& flaws,
        const std::function<bool(const Flaw&)>& notify_flaw,
        downward::utils::CountdownTimer& timer) = 0;

    virtual std::string get_name() = 0;
};

bool collect_flaws(
    downward::PreconditionsProxy facts,
    const downward::State& state,
    std::vector<Flaw>& flaws,
    const std::function<bool(const Flaw&)>& accept_flaw);

bool collect_flaws(
    downward::GoalsProxy facts,
    const downward::State& state,
    std::vector<Flaw>& flaws,
    const std::function<bool(const Flaw&)>& accept_flaw);

} // namespace probfd::pdbs::cegar

#endif // PROBFD_PDBS_CEGAR_FLAW_FINDING_STRATEGY_H
