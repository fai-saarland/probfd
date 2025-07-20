#ifndef PROBFD_POLICY_PICKERS_VDIFF_TIEBREAKER_H
#define PROBFD_POLICY_PICKERS_VDIFF_TIEBREAKER_H

#include "probfd/policy_pickers/stable_policy_picker.h"

namespace probfd::policy_pickers {

template <typename State, typename Action>
class VDiffTiebreaker
    : public StablePolicyPicker<State, Action, VDiffTiebreaker<State, Action>> {
    const value_t factor_;

public:
    explicit VDiffTiebreaker(bool stable_policy, bool favor_large_gaps);

    int pick_index(
        MDP<State, Action>& mdp,
        std::optional<Action> prev_policy,
        const std::vector<TransitionTail<Action>>& greedy_transitions,
        algorithms::StateProperties& properties) override;
};

} // namespace probfd::policy_pickers

#include "probfd/policy_pickers/vdiff_tiebreaker_impl.h"

#endif // PROBFD_POLICY_PICKERS_VDIFF_TIEBREAKER_H