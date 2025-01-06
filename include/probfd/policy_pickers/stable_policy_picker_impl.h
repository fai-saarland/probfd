#include "probfd/distribution.h"
#include "probfd/transition.h"

namespace probfd::policy_pickers {

template <typename State, typename Action, class Derived>
StablePolicyPicker<State, Action, Derived>::StablePolicyPicker(
    bool stable_policy)
    : stable_policy_(stable_policy)
{
}

template <typename State, typename Action, class Derived>
int StablePolicyPicker<State, Action, Derived>::pick_index(
    MDP<State, Action>& mdp,
    std::optional<Action> previous_greedy,
    const std::vector<Transition<Action>>& greedy_transitions,
    algorithms::StateProperties& properties)
{
    if (stable_policy_) {
        for (unsigned i = 0; i < greedy_transitions.size(); ++i) {
            if (greedy_transitions[i].action == previous_greedy) {
                return i;
            }
        }
    }

    return static_cast<Derived*>(this)
        ->pick_index(mdp, previous_greedy, greedy_transitions, properties);
}

} // namespace probfd::policy_pickers
