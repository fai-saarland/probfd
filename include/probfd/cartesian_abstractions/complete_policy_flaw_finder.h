#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_COMPLETE_POLICY_FLAW_FINDER_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_COMPLETE_POLICY_FLAW_FINDER_H

#include "probfd/cartesian_abstractions/flaw.h"
#include "probfd/cartesian_abstractions/policy_flaw_finder.h"
#include "probfd/cartesian_abstractions/types.h"

#include <optional>
#include <vector>

// Forward Declarations
namespace downward::utils {
class CountdownTimer;
class LogProxy;
} // namespace utils

namespace probfd {
class ProbabilisticTask;
}

namespace probfd::cartesian_abstractions {
class CartesianAbstraction;
}

namespace probfd::cartesian_abstractions {

class CompletePolicyFlawFinder : public PolicyFlawFinder {
    int max_search_states_;

public:
    explicit CompletePolicyFlawFinder(int max_search_states);

    std::optional<Flaw> find_flaw(
        const ProbabilisticTask& task,
        const std::vector<int>& domain_sizes,
        CartesianAbstraction& abstraction,
        Solution& policy,
        downward::utils::LogProxy& log,
        downward::utils::CountdownTimer& timer) override;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_COMPLETE_POLICY_FLAW_FINDER_H
