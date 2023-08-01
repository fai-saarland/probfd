#ifndef PROBFD_HEURISTICS_CARTESIAN_COMPLETE_POLICY_FLAW_FINDER_H
#define PROBFD_HEURISTICS_CARTESIAN_COMPLETE_POLICY_FLAW_FINDER_H

#include "probfd/heuristics/cartesian/policy_flaw_finder.h"

namespace probfd {
namespace heuristics {
namespace cartesian {

class CompletePolicyFlawFinder : public PolicyFlawFinder {
public:
    std::optional<Flaw> find_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        Abstraction& abstraction,
        Solution& policy,
        utils::CountdownTimer& timer,
        utils::LogProxy& log,
        const std::vector<int>& domain_sizes,
        int max_search_states) override;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif