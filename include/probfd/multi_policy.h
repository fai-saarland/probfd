#ifndef PROBFD_MULTI_POLICY_H
#define PROBFD_MULTI_POLICY_H

#include "probfd/interval.h"

#include <ranges>
#include <vector>

namespace probfd {

/// Stores an action and a bounding interval for the Q* value of this action.
template <typename Action>
struct PolicyDecision {
    Action action; ///< The action.
    Interval q_value_interval =
        Interval(-INFINITE_VALUE, INFINITE_VALUE); ///< The Q* value interval.
};

template <typename Action>
PolicyDecision(Action b, Interval e) -> PolicyDecision<Action>;

/**
 * @brief Represents a policy that can specify a set of actions for a state.
 *
 * A multi policy is a mapping \f$ S \rightharpoonup 2^A \f$ from states
 * \f$ S \f$ to subsets actions \f$ 2^A \f$ (including the empty set).
 * Additionally, provides bounding intervals for the Q* value of each specified
 * action.
 *
 * @tparam State - The state type of the underlying state space.
 * @tparam Action - The action type of the underlying state space.
 */
template <typename State, typename Action>
class MultiPolicy {
public:
    virtual ~MultiPolicy() = default;

    /// Retrieves the actions and their optimal state value intervals specified
    /// by the policy for a given state.
    virtual std::vector<PolicyDecision<Action>>
    get_decisions(const State& state) const = 0;
};

} // namespace probfd

#endif