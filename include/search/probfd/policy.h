#ifndef PROBFD_POLICY_H
#define PROBFD_POLICY_H

#include "probfd/interval.h"

#include <optional>
#include <vector>

namespace probfd {

/// Stores an action and a bounding interval for the Q* value of this action.
template <typename Action>
struct PolicyDecision {
    Action action;                 ///< The action.
    Interval q_value_interval;     ///< The Q* value interval.

    PolicyDecision(
        Action action,
        Interval q_value_interval = Interval(-INFINITE_VALUE, INFINITE_VALUE))
        : action(action)
        , q_value_interval(q_value_interval)
    {
    }
};

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
class PartialPolicy {
public:
    virtual ~PartialPolicy() = default;

    /// Retrives the action and optimal state value interval specified by the
    /// policy for a given state.
    virtual std::optional<PolicyDecision<Action>>
    get_decision(const State& state) = 0;
};

} // namespace probfd

#endif