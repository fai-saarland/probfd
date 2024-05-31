
#ifndef PROBFD_POLICIES_EMPTY_POLICY_H
#define PROBFD_POLICIES_EMPTY_POLICY_H

#include "probfd/policy.h"

namespace probfd::policies {

template <typename State, typename Action>
class EmptyPolicy : public Policy<State, Action> {
public:
    /// Retrieves the action and optimal state value interval specified by the
    /// policy for a given state.
    std::optional<PolicyDecision<Action>>
    get_decision(const State&) const override
    {
        return std::nullopt;
    }

    void print(
        std::ostream&,
        std::function<void(const State&, std::ostream&)>,
        std::function<void(const Action&, std::ostream&)>) override
    {
    }
};

} // namespace probfd::policies

#endif