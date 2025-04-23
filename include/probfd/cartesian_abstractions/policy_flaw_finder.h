#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_POLICY_FLAW_FINDER_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_POLICY_FLAW_FINDER_H

#include "probfd/cartesian_abstractions/flaw.h"
#include "probfd/cartesian_abstractions/types.h"

#include <memory>
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
class CartesianHeuristic;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

class PolicyFlawFinder {
public:
    virtual ~PolicyFlawFinder() = default;

    virtual std::optional<Flaw> find_flaw(
        const ProbabilisticTask& task,
        const std::vector<int>& domain_sizes,
        CartesianAbstraction& abstraction,
        Solution& policy,
        downward::utils::LogProxy& log,
        downward::utils::CountdownTimer& timer) = 0;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_POLICY_FLAW_FINDER_H
