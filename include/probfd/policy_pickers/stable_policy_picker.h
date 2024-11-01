#ifndef PROBFD_POLICY_PICKERS_STABLE_POLICY_PICKER_H
#define PROBFD_POLICY_PICKERS_STABLE_POLICY_PICKER_H

#include "probfd/algorithms/policy_picker.h"

namespace probfd::policy_pickers {

/**
 * @brief CRTP base class for stable policy picker implementations.
 */
template <typename State, typename Action, class Derived>
class StablePolicyPicker : public algorithms::PolicyPicker<State, Action> {
    bool stable_policy_;

public:
    explicit StablePolicyPicker(bool stable_policy);

    int pick_index(
        MDP<State, Action>& mdp,
        std::optional<Action> previous_greedy,
        const std::vector<Transition<Action>>& greedy_transitions,
        algorithms::StateProperties& properties) override;
};

} // namespace probfd::policy_pickers

#include "probfd/policy_pickers/stable_policy_picker_impl.h"

#endif