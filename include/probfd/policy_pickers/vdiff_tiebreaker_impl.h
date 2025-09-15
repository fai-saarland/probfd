#include "probfd/policy_pickers/vdiff_tiebreaker.h"

#include "probfd/algorithms/state_properties.h"

#include "probfd/interval.h"

namespace probfd::policy_pickers {

template <typename State, typename Action>
VDiffTiebreaker<State, Action>::VDiffTiebreaker(
    bool stable_policy,
    bool favor_large_gaps)
    : VDiffTiebreaker::StablePolicyPicker(stable_policy)
    , factor_(favor_large_gaps ? -1 : 1)
{
}

template <typename State, typename Action>
int VDiffTiebreaker<State, Action>::pick_index(
    MDP<State, Action>&,
    std::optional<Action>,
    const std::vector<TransitionTail<Action>>& greedy_transitions,
    algorithms::StateProperties& properties)
{
    auto it = std::ranges::min_element(
        greedy_transitions,
        {},
        [&properties,
         factor = factor_](const TransitionTail<Action>& t) {
            return t.successor_dist.non_source_successor_dist.expectation(
                [&](StateID id) {
                    return factor * properties.lookup_bounds(id).length();
                });
        });

    return std::distance(greedy_transitions.begin(), it);
}

} // namespace probfd::policy_pickers
