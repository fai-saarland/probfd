#ifndef PROBFD_POLICY_PICKERS_STABLE_POLICY_PICKER_H
#define PROBFD_POLICY_PICKERS_STABLE_POLICY_PICKER_H

#include "probfd/engines/fdr_types.h"
#include "probfd/engines/policy_picker.h"

#include "probfd/distribution.h"
#include "probfd/transition.h"

namespace probfd {
namespace policy_pickers {

/**
 * @brief CRTP base class for stable policy picker implementations.
 */
template <typename State, typename Action, class Derived>
class StablePolicyPicker : public engines::PolicyPicker<State, Action> {
    bool stable_policy_;

public:
    explicit StablePolicyPicker(bool stable_policy)
        : stable_policy_(stable_policy)
    {
    }

    int pick_index(
        MDP<State, Action>& mdp,
        StateID state_id,
        std::optional<Action> previous_greedy,
        const std::vector<Transition<Action>>& greedy_transitions,
        engines::StateProperties& properties) override
    {
        if (stable_policy_) {
            for (unsigned i = 0; i < greedy_transitions.size(); ++i) {
                if (greedy_transitions[i].action == previous_greedy) {
                    return i;
                }
            }
        }

        return static_cast<Derived*>(this)->pick_index(
            mdp,
            state_id,
            previous_greedy,
            greedy_transitions,
            properties);
    }
};

template <class Derived>
using TaskStablePolicyPicker = StablePolicyPicker<State, OperatorID, Derived>;

} // namespace policy_pickers
} // namespace probfd

#endif