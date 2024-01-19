#ifndef PROBFD_POLICY_PICKER_RANDOM_TIEBREAKER_H
#define PROBFD_POLICY_PICKER_RANDOM_TIEBREAKER_H

#include "probfd/policy_pickers/stable_policy_picker.h"

#include <memory>

// Forward Declarations
namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace utils {
class RandomNumberGenerator;
}

namespace probfd::policy_pickers {

template <typename State, typename Action>
class RandomTiebreaker
    : public StablePolicyPicker<
          State,
          Action,
          RandomTiebreaker<State, Action>> {
    std::shared_ptr<utils::RandomNumberGenerator> rng_;

public:
    explicit RandomTiebreaker(const plugins::Options&);
    explicit RandomTiebreaker(
        bool stable_policy,
        std::shared_ptr<utils::RandomNumberGenerator> rng);

    int pick_index(
        MDP<State, Action>& mdp,
        StateID state,
        std::optional<Action> prev_policy,
        const std::vector<Transition<Action>>& greedy_transitions,
        algorithms::StateProperties& properties);
};

} // namespace probfd::policy_pickers

#include "probfd/policy_pickers/random_tiebreaker_impl.h"

#endif // PROBFD_POLICY_PICKER_RANDOM_TIEBREAKER_H