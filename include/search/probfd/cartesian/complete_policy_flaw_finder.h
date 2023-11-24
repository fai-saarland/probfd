#ifndef PROBFD_CARTESIAN_COMPLETE_POLICY_FLAW_FINDER_H
#define PROBFD_CARTESIAN_COMPLETE_POLICY_FLAW_FINDER_H

#include "probfd/cartesian/policy_flaw_finder.h"

namespace probfd {
namespace cartesian {

class CompletePolicyFlawFinder : public PolicyFlawFinder {
    int max_search_states;

public:
    CompletePolicyFlawFinder(int max_search_states);

    std::optional<Flaw> find_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        const std::vector<int>& domain_sizes,
        Abstraction& abstraction,
        Solution& policy,
        utils::LogProxy& log,
        utils::CountdownTimer& timer) override;
};

} // namespace cartesian
} // namespace probfd

#endif // PROBFD_CARTESIAN_COMPLETE_POLICY_FLAW_FINDER_H
