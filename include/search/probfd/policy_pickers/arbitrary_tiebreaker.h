#ifndef PROBFD_POLICY_PICKER_ARBITRARY_TIEBREAKER_H
#define PROBFD_POLICY_PICKER_ARBITRARY_TIEBREAKER_H

#include "probfd/policy_pickers/stable_policy_picker.h"

#include "downward/plugins/options.h"

namespace probfd {
namespace policy_pickers {

template <typename State, typename Action>
class ArbitraryTiebreaker
    : public StablePolicyPicker<
          State,
          Action,
          ArbitraryTiebreaker<State, Action>> {
public:
    explicit ArbitraryTiebreaker(const plugins::Options& opts)
        : ArbitraryTiebreaker(opts.get<bool>("stable_policy"))
    {
    }

    explicit ArbitraryTiebreaker(bool stable_policy)
        : StablePolicyPicker<State, Action, ArbitraryTiebreaker<State, Action>>(
              stable_policy)
    {
    }

    int pick_index(
        engine_interfaces::MDP<State, Action>&,
        StateID,
        std::optional<Action>,
        const std::vector<Action>&,
        const std::vector<Distribution<StateID>>&,
        engine_interfaces::StateProperties&)
    {
        return 0;
    }
};

} // namespace policy_pickers
} // namespace probfd

#endif // __ARBITRARY_TIEBREAKER_H__