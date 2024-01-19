#ifndef PROBFD_CARTESIAN_POLICY_GENERATOR_H
#define PROBFD_CARTESIAN_POLICY_GENERATOR_H

#include "probfd/cartesian_abstractions/types.h"

#include <memory>

// Forward Declarations
namespace utils {
class CountdownTimer;
} // namespace utils

namespace probfd::cartesian_abstractions {
class Abstraction;
class CartesianHeuristic;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

class PolicyGenerator {
public:
    virtual ~PolicyGenerator() = default;

    virtual std::unique_ptr<Solution> find_solution(
        Abstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& time_limit) = 0;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_POLICY_GENERATOR_H
