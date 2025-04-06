#ifndef PROBFD_POLICY_PICKERS_RANDOM_TIEBREAKER_H
#define PROBFD_POLICY_PICKERS_RANDOM_TIEBREAKER_H

#include "probfd/policy_pickers/stable_policy_picker.h"

#include <memory>

// Forward Declarations
namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd::policy_pickers {

template <typename State, typename Action>
class RandomTiebreaker
    : public StablePolicyPicker<
          State,
          Action,
          RandomTiebreaker<State, Action>> {
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;

public:
    explicit RandomTiebreaker(bool stable_policy, int random_seed);
    explicit RandomTiebreaker(
        bool stable_policy,
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng);

    int pick_index(
        MDP<State, Action>& mdp,
        std::optional<Action> prev_policy,
        const std::vector<TransitionTail<Action>>& greedy_transitions,
        algorithms::StateProperties& properties) override;
};

} // namespace probfd::policy_pickers

#include "probfd/policy_pickers/random_tiebreaker_impl.h"

#endif // PROBFD_POLICY_PICKERS_RANDOM_TIEBREAKER_H