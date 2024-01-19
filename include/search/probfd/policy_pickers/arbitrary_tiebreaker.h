#ifndef PROBFD_POLICY_PICKER_ARBITRARY_TIEBREAKER_H
#define PROBFD_POLICY_PICKER_ARBITRARY_TIEBREAKER_H

#include "probfd/policy_pickers/stable_policy_picker.h" // IWYU pragma: export

// Forward Declarations
namespace plugins {
class Options;
}

namespace probfd::policy_pickers {

template <typename State, typename Action>
class ArbitraryTiebreaker
    : public StablePolicyPicker<
          State,
          Action,
          ArbitraryTiebreaker<State, Action>> {
public:
    explicit ArbitraryTiebreaker(const plugins::Options& opts);
    explicit ArbitraryTiebreaker(bool stable_policy);

    int pick_index(
        MDP<State, Action>&,
        StateID,
        std::optional<Action>,
        const std::vector<Transition<Action>>&,
        algorithms::StateProperties&) override;
};

} // namespace probfd::policy_pickers

#include "probfd/policy_pickers/arbitrary_tiebreaker_impl.h"

#endif // PROBFD_POLICY_PICKER_ARBITRARY_TIEBREAKER_H