#ifndef PROBFD_POLICIES_MAP_POLICY_H
#define PROBFD_POLICIES_MAP_POLICY_H

#include "probfd/policy.h"
#include "probfd/state_id.h"
#include "probfd/state_space.h"

#include <unordered_map>

namespace probfd::policies {

template <typename State, typename Action>
class MapPolicy : public Policy<State, Action> {
    StateSpace<State, Action>* state_space_;
    std::unordered_map<StateID, PolicyDecision<Action>> mapping_;

public:
    explicit MapPolicy(StateSpace<State, Action>* state_space)
        : state_space_(state_space)
    {
    }

    std::optional<PolicyDecision<Action>>
    get_decision(const State& state) const override
    {
        const auto state_id = state_space_->get_state_id(state);
        auto it = mapping_.find(state_id);
        if (it != mapping_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    PolicyDecision<Action>&
    emplace_decision(StateID state_id, Action action, Interval interval)
    {
        return mapping_.emplace(state_id, PolicyDecision(action, interval))
            .first->second;
    }

    PolicyDecision<Action>& operator[](StateID state_id)
    {
        return mapping_[state_id];
    }

    const PolicyDecision<Action>& operator[](StateID state_id) const
    {
        return mapping_.find(state_id)->second;
    }
};

} // namespace probfd::policies

#endif