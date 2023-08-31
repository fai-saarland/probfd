#ifndef PROBFD_POLICY_H
#define PROBFD_POLICY_H

#include "probfd/multi_policy.h"

#include <optional>
#include <vector>

namespace probfd {

/**
 * @brief Represents a deterministic, stationary, partial policy.
 *
 * Represents a deterministic, stationary, partial policy, i.e., a
 * partial mapping \f$ S \rightharpoonup A \f$ from states \f$ S \f$ to actions
 * \f$ A \f$. Additionally, provides bounding intervals for the Q* value of
 * the policy actions.
 *
 * @tparam State - The state type of the underlying state space.
 * @tparam Action - The action type of the underlying state space.
 */
template <typename State, typename Action>
class Policy : public MultiPolicy<State, Action> {
public:
    virtual ~Policy() = default;

    /// Retrives the action and optimal state value interval specified by the
    /// policy for a given state.
    virtual std::optional<PolicyDecision<Action>>
    get_decision(const State& state) const = 0;

    std::vector<PolicyDecision<Action>>
    get_decisions(const State& state) const override
    {
        std::optional decision = this->get_decision(state);
        std::vector<PolicyDecision<Action>> decisions;
        if (decision) decisions.emplace_back(std::move(*decision));
        return decisions;
    }
};

} // namespace probfd

#endif