#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_POLICY_GENERATOR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_POLICY_GENERATOR_H

#include "probfd/cartesian_abstractions/types.h"

#include <memory>

// Forward Declarations
namespace downward::utils {
class CountdownTimer;
} // namespace utils

namespace probfd::cartesian_abstractions {
class CartesianAbstraction;
class CartesianHeuristic;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

class PolicyGenerator {
public:
    virtual ~PolicyGenerator() = default;

    virtual std::unique_ptr<Solution> find_solution(
        CartesianAbstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        downward::utils::CountdownTimer& time_limit) = 0;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_POLICY_GENERATOR_H
