#ifndef MDPS_POLICY_PICKER_ARBITRARY_TIEBREAKER_H
#define MDPS_POLICY_PICKER_ARBITRARY_TIEBREAKER_H

#include "probfd/engine_interfaces/policy_picker.h"

namespace probfd {
namespace policy_pickers {

template <typename Action>
class ArbitraryTiebreaker : public engine_interfaces::PolicyPicker<Action> {
protected:
    virtual int pick(
        const StateID&,
        const ActionID&,
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