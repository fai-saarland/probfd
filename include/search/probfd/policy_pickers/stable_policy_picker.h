#ifndef PROBFD_POLICY_PICKERS_STABLE_POLICY_PICKER_H
#define PROBFD_POLICY_PICKERS_STABLE_POLICY_PICKER_H

#include "probfd/engine_interfaces/policy_picker.h"

namespace probfd {
namespace policy_pickers {

/**
 * @brief CRTP base class for stable policy picker implementations.
 */
template <typename State, typename Action, class Derived>
class StablePolicyPicker
    : public engine_interfaces::PolicyPicker<State, Action> {
    bool stable_policy_;

public:
    explicit StablePolicyPicker(bool stable_policy)
        : stable_policy_(stable_policy)
    {
    }

    int pick_index(
        engine_interfaces::StateSpace<State, Action>& state_space,
        StateID state_id,
        std::optional<Action> previous_greedy,
        const std::vector<Action>& greedy_action_candidates,
        const std::vector<Distribution<StateID>>& candidate_successors,
        engine_interfaces::StateProperties& properties) override
    {
        if (stable_policy_) {
            for (unsigned i = 0; i < greedy_action_candidates.size(); ++i) {
                if (greedy_action_candidates[i] == previous_greedy) {
                    return i;
                }
            }
        }

        return static_cast<Derived*>(this)->pick_index(
            state_space,
            state_id,
            previous_greedy,
            greedy_action_candidates,
            candidate_successors,
            properties);
    }
};

template <class Derived>
using TaskStablePolicyPicker = StablePolicyPicker<State, OperatorID, Derived>;

} // namespace policy_pickers
} // namespace probfd

#endif