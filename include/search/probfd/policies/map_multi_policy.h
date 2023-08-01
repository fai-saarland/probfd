#ifndef PROBFD_POLICIES_MAP_MULTI_POLICY_H
#define PROBFD_POLICIES_MAP_MULTI_POLICY_H

#include "probfd/multi_policy.h"
#include "probfd/types.h"

#include <unordered_map>

namespace probfd {
namespace policies {

template <typename State, typename Action>
class MapMultiPolicy : public MultiPolicy<State, Action> {
    engine_interfaces::StateSpace<State, Action>* state_space;
    std::unordered_map<StateID, std::vector<PolicyDecision<Action>>> mapping;

public:
    explicit MapMultiPolicy(
        engine_interfaces::StateSpace<State, Action>* state_space)
        : state_space(state_space)
    {
    }

    std::vector<PolicyDecision<Action>>
    get_decisions(const State& state) const override
    {
        const auto state_id = state_space->get_state_id(state);
        auto it = mapping.find(state_id);
        if (it != mapping.end()) {
            return it->second;
        }
        return {};
    }

    std::vector<PolicyDecision<Action>>& emplace_decisions(
        StateID state_id,
        std::vector<PolicyDecision<Action>> decisions)
    {
        return mapping.emplace(state_id, std::move(decisions)).first->second;
    }

    PolicyDecision<Action>&
    emplace_decision(StateID state_id, Action action, Interval interval)
    {
        return mapping.emplace(state_id, PolicyDecision(action, interval))
            .first->second;
    }

    std::vector<PolicyDecision<Action>>& operator[](StateID state_id)
    {
        return mapping[state_id];
    }

    const std::vector<PolicyDecision<Action>>&
    operator[](StateID state_id) const
    {
        return mapping.find(state_id)->second;
    }
};

} // namespace policies
} // namespace probfd

#endif