#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/plugins/options.h"

namespace probfd {
namespace policy_pickers {

template <typename State, typename Action>
RandomTiebreaker<State, Action>::RandomTiebreaker(const plugins::Options& opts)
    : RandomTiebreaker(
          opts.get<bool>("stable_policy"),
          utils::parse_rng_from_options(opts))
{
}

template <typename State, typename Action>
RandomTiebreaker<State, Action>::RandomTiebreaker(
    bool stable_policy,
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : RandomTiebreaker::StablePolicyPicker(stable_policy)
    , rng(rng)
{
}

template <typename State, typename Action>
int RandomTiebreaker<State, Action>::pick_index(
    MDP<State, Action>&,
    StateID,
    std::optional<Action>,
    const std::vector<Transition<Action>>& greedy_transitions,
    algorithms::StateProperties&)
{
    return rng->random(greedy_transitions.size());
}

} // namespace policy_pickers
} // namespace probfd
