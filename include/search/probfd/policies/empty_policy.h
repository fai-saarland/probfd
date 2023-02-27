
#ifndef PROBFD_POLICIES_EMPTY_POLICY_H
#define PROBFD_POLICIES_EMPTY_POLICY_H

#include "probfd/policy.h"

namespace probfd {
namespace policies {

template <typename State, typename Action>
class EmptyPolicy : public PartialPolicy<State, Action> {
public:
    ~EmptyPolicy() override = default;

    /// Retrives the action and optimal state value interval specified by the
    /// policy for a given state.
    std::optional<PolicyDecision<Action>> get_decision(const State&) override
    {
        return std::nullopt;
    }
};

} // namespace policies
} // namespace probfd

#endif