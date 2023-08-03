#ifndef PROBFD_POLICIES_VECTOR_MULTI_POLICY_H
#define PROBFD_POLICIES_VECTOR_MULTI_POLICY_H

#include "probfd/multi_policy.h"
#include "probfd/types.h"

#include <unordered_map>

namespace probfd {
namespace policies {

template <typename State, typename Action>
class VectorMultiPolicy : public MultiPolicy<State, Action> {
    StateSpace<State, Action>* state_space;
    std::vector<std::vector<PolicyDecision<Action>>> decisions_per_state;

public:
    explicit VectorMultiPolicy(
        StateSpace<State, Action>* state_space,
        size_t num_states)
        : state_space(state_space)
        , decisions_per_state(num_states, std::vector<PolicyDecision<Action>>{})
    {
    }

    std::vector<PolicyDecision<Action>>
    get_decisions(const State& state) const override
    {
        const auto state_id = state_space->get_state_id(state);
        return decisions_per_state[state_id];
    }

    std::vector<PolicyDecision<Action>>& operator[](StateID state_id)
    {
        return decisions_per_state[state_id];
    }

    const std::vector<PolicyDecision<Action>>&
    operator[](StateID state_id) const
    {
        return decisions_per_state[state_id];
    }
};

} // namespace policies
} // namespace probfd

#endif