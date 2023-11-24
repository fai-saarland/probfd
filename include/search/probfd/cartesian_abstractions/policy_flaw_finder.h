#ifndef PROBFD_CARTESIAN_POLICY_FLAW_FINDER_H
#define PROBFD_CARTESIAN_POLICY_FLAW_FINDER_H

#include "probfd/cartesian_abstractions/flaw.h"
#include "probfd/cartesian_abstractions/types.h"

#include <memory>
#include <optional>
#include <vector>

namespace utils {
class CountdownTimer;
class LogProxy;
} // namespace utils

namespace probfd {
class ProbabilisticTaskProxy;

namespace cartesian_abstractions {

class Abstraction;
class CartesianCostFunction;
class CartesianHeuristic;

class PolicyFlawFinder {
public:
    virtual ~PolicyFlawFinder() = default;

    virtual std::optional<Flaw> find_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        const std::vector<int>& domain_sizes,
        Abstraction& abstraction,
        Solution& policy,
        utils::LogProxy& log,
        utils::CountdownTimer& timer) = 0;
};

} // namespace cartesian_abstractions
} // namespace probfd

#endif // PROBFD_CARTESIAN_POLICY_FLAW_FINDER_H
