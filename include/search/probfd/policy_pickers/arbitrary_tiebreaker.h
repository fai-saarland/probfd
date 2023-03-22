#ifndef PROBFD_POLICY_PICKER_ARBITRARY_TIEBREAKER_H
#define PROBFD_POLICY_PICKER_ARBITRARY_TIEBREAKER_H

#include "probfd/engine_interfaces/policy_picker.h"

namespace probfd {
namespace policy_pickers {

template <typename State, typename Action>
class ArbitraryTiebreaker
    : public engine_interfaces::PolicyPicker<State, Action> {
protected:
    virtual int pick(
        engine_interfaces::StateSpace<State, Action>&,
        StateID,
        ActionID,
        const std::vector<Action>&,
        const std::vector<Distribution<StateID>>&,
        engine_interfaces::HeuristicSearchInterface&) override
    {
        return 0;
    }
};

} // namespace policy_pickers
} // namespace probfd

#endif // __ARBITRARY_TIEBREAKER_H__