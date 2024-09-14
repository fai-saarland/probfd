#include <utility>

#include "downward/utils/rng.h"

namespace probfd::policy_pickers {

template <typename State, typename Action>
RandomTiebreaker<State, Action>::RandomTiebreaker(
    bool stable_policy,
    int random_seed)
    : RandomTiebreaker(
          stable_policy,
          std::make_shared<utils::RandomNumberGenerator>(random_seed))
{
}

template <typename State, typename Action>
RandomTiebreaker<State, Action>::RandomTiebreaker(
    bool stable_policy,
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : RandomTiebreaker::StablePolicyPicker(stable_policy)
    , rng_(std::move(rng))
{
}

template <typename State, typename Action>
int RandomTiebreaker<State, Action>::pick_index(
    MDP<State, Action>&,
    std::optional<Action>,
    const std::vector<Transition<Action>>& greedy_transitions,
    algorithms::StateProperties&)
{
    return rng_->random(greedy_transitions.size());
}

} // namespace probfd::policy_pickers
