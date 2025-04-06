#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_ILAO_POLICY_GENERATOR_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_ILAO_POLICY_GENERATOR_H

#include "probfd/cartesian_abstractions/policy_generator.h"
#include "probfd/cartesian_abstractions/types.h"

#include "probfd/aliases.h"

#include <memory>

// Forward Declarations
namespace downward::utils {
class CountdownTimer;
}

namespace probfd::quotients {
template <typename, typename>
struct QuotientState;
template <typename>
struct QuotientAction;
} // namespace probfd::quotients

namespace probfd::policy_pickers {
template <typename State, typename Action>
class ArbitraryTiebreaker;
}

namespace probfd::cartesian_abstractions {
class AbstractState;
class CartesianAbstraction;
class CartesianHeuristic;
struct ProbabilisticTransition;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

/**
 * @brief Find an optimal policy using ILAO*.
 */
class ILAOPolicyGenerator : public PolicyGenerator {
    const value_t convergence_epsilon_;

    std::shared_ptr<policy_pickers::ArbitraryTiebreaker<
        quotients::QuotientState<int, const ProbabilisticTransition*>,
        quotients::QuotientAction<const ProbabilisticTransition*>>>
        picker_;

public:
    explicit ILAOPolicyGenerator(value_t convergence_epsilon);

    std::unique_ptr<Solution> find_solution(
        CartesianAbstraction& abstraction,
        const AbstractState* init_id,
        CartesianHeuristic& heuristic,
        downward::utils::CountdownTimer& time_limit) override;
};

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_ILAO_POLICY_GENERATOR_H
