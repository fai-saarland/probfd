#ifndef PROBFD_POLICIES_MAP_POLICY_H
#define PROBFD_POLICIES_MAP_POLICY_H

#include "probfd/policy.h"
#include "probfd/state_space.h"
#include "probfd/types.h"

#include <unordered_map>

namespace probfd {
namespace policies {

template <typename State, typename Action>
class MapPolicy : public PartialPolicy<State, Action> {
    StateSpace<State, Action>* state_space;
    std::unordered_map<StateID, PolicyDecision<Action>> mapping;

public:
    explicit MapPolicy(StateSpace<State, Action>* state_space)
        : state_space(state_space)
    {
    }

    ~MapPolicy() override = default;

    std::optional<PolicyDecision<Action>>
    get_decision(const State& state) const override
    {
        const auto state_id = state_space->get_state_id(state);
        auto it = mapping.find(state_id);
        if (it != mapping.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    PolicyDecision<Action>&
    emplace_decision(StateID state_id, Action action, Interval interval)
    {
        return mapping.emplace(state_id, PolicyDecision(action, interval))
            .first->second;
    }

    PolicyDecision<Action>& operator[](StateID state_id)
    {
        return mapping[state_id];
    }

    const PolicyDecision<Action>& operator[](StateID state_id) const
    {
        return mapping.find(state_id)->second;
    }
};

} // namespace policies
} // namespace probfd

#endif