#ifndef MDPS_POLICY_PICKER_VDIFF_TIEBREAKER_H
#define MDPS_POLICY_PICKER_VDIFF_TIEBREAKER_H

#include "probfd/engine_interfaces/policy_picker.h"

namespace probfd {

/// Namespace dedicated to policy tiebreaker implementations.
namespace policy_tiebreaking {

class VDiffTiebreaker : public TaskPolicyPicker {
    const value_type::value_t favor_large_gaps_;

public:
    VDiffTiebreaker(value_type::value_t favor_large_gaps_);

    virtual int pick(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<OperatorID>& action_choices,
        const std::vector<Distribution<StateID>>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface) override;
};

} // namespace policy_tiebreaking
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__