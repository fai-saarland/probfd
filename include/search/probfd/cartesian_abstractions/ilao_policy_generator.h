#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_ILAO_POLICY_GENERATOR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_ILAO_POLICY_GENERATOR_H

#include "probfd/cartesian_abstractions/policy_generator.h"
#include "probfd/cartesian_abstractions/types.h"

#include <memory>

namespace utils {
class CountdownTimer;
}

namespace probfd {

namespace quotients {
template <typename, typename>
struct QuotientState;
template <typename>
struct QuotientAction;
} // namespace quotients

namespace policy_pickers {
template <typename State, typename Action>
class ArbitraryTiebreaker;
}

namespace cartesian_abstractions {

class AbstractState;
class Abstraction;
class CartesianHeuristic;

struct ProbabilisticTransition;

/**
 * @brief Find an optimal policy using ILAO*.
 */
class ILAOPolicyGenerator : public PolicyGenerator {
    std::shared_ptr<policy_pickers::ArbitraryTiebreaker<
        quotients::QuotientState<int, const ProbabilisticTransition*>,
        quotients::QuotientAction<const ProbabilisticTransition*>>>
        picker;

public:
    ILAOPolicyGenerator();

    std::unique_ptr<Solution> find_solution(
        Abstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        utils::CountdownTimer& time_limit) override;
};

} // namespace cartesian_abstractions
} // namespace probfd

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_ILAO_POLICY_GENERATOR_H
