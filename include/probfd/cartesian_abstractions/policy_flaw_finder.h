#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_POLICY_FLAW_FINDER_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_POLICY_FLAW_FINDER_H

#include "probfd/cartesian_abstractions/flaw.h"
#include "probfd/cartesian_abstractions/types.h"

#include <memory>
#include <optional>
#include <vector>

// Forward Declarations
namespace utils {
class CountdownTimer;
class LogProxy;
} // namespace utils

namespace probfd {
class ProbabilisticTaskProxy;
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
        const ProbabilisticTaskProxy& task_proxy,
        const std::vector<int>& domain_sizes,
        CartesianAbstraction& abstraction,
        Solution& policy,
        utils::LogProxy& log,
        utils::CountdownTimer& timer) = 0;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_POLICY_FLAW_FINDER_H
