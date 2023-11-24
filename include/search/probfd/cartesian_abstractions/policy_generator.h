#ifndef PROBFD_CARTESIAN_POLICY_GENERATOR_H
#define PROBFD_CARTESIAN_POLICY_GENERATOR_H

#include "probfd/cartesian_abstractions/types.h"

#include <memory>

namespace utils {
class CountdownTimer;
} // namespace utils

namespace probfd {
namespace cartesian_abstractions {

class Abstraction;
class CartesianCostFunction;
class CartesianHeuristic;

class PolicyGenerator {
public:
    virtual ~PolicyGenerator() = default;

    virtual std::unique_ptr<Solution> find_solution(
        Abstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& time_limit) = 0;
};

} // namespace cartesian_abstractions
} // namespace probfd

#endif // PROBFD_CARTESIAN_POLICY_GENERATOR_H
