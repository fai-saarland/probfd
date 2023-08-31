#ifndef PROBFD_POLICY_PICKER_VDIFF_TIEBREAKER_H
#define PROBFD_POLICY_PICKER_VDIFF_TIEBREAKER_H

#include "probfd/policy_pickers/stable_policy_picker.h"

namespace plugins {
class Options;
} // namespace plugins

namespace probfd {
namespace policy_pickers {

template <typename State, typename Action>
class VDiffTiebreaker
    : public StablePolicyPicker<State, Action, VDiffTiebreaker<State, Action>> {
    const value_t favor_large_gaps_;

public:
    explicit VDiffTiebreaker(const plugins::Options&);
    explicit VDiffTiebreaker(bool stable_policy, value_t favor_large_gaps_);

    int pick_index(
        MDP<State, Action>& mdp,
        StateID state,
        std::optional<Action> prev_policy,
        const std::vector<Transition<Action>>& greedy_transitions,
        algorithms::StateProperties& properties);
};

} // namespace policy_pickers
} // namespace probfd

#include "probfd/policy_pickers/vdiff_tiebreaker_impl.h"

#endif // __VDIFF_TIEBREAKER_H__