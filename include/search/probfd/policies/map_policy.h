#ifndef PROBFD_POLICIES_MAP_POLICY_H
#define PROBFD_POLICIES_MAP_POLICY_H

#include "probfd/policy.h"
#include "probfd/types.h"

#include <unordered_map>

namespace probfd {
namespace policies {

template <typename State, typename Action>
class MapPolicy : public PartialPolicy<State, Action> {
    engine_interfaces::StateSpace<State, Action>* state_space;
    std::unordered_map<StateID, std::pair<ActionID, Interval>> mapping;

public:
    explicit MapPolicy(
        engine_interfaces::StateSpace<State, Action>* state_space)
        : state_space(state_space)
    {
    }

    ~MapPolicy() override = default;

    std::optional<PolicyDecision<Action>>
    get_decision(const State& state) override
    {
        const auto state_id = state_space->get_state_id(state);
        auto it = mapping.find(state_id);
        if (it != mapping.end()) {
            auto& [action_id, bound] = it->second;
            return PolicyDecision(
                state_space->get_action(state_id, action_id),
                bound);
        }
        return std::nullopt;
    }

    std::pair<ActionID, Interval>&
    emplace_decision(StateID state_id, ActionID action_id, Interval interval)
    {
        return mapping.emplace(state_id, std::make_pair(action_id, interval))
            .first->second;
    }

    std::pair<ActionID, Interval>& operator[](StateID state_id)
    {
        return mapping[state_id];
    }

    const std::pair<ActionID, Interval>& operator[](StateID state_id) const
    {
        return mapping.find(state_id)->second;
    }
};

} // namespace policies
} // namespace probfd

#endif