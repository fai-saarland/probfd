#ifndef MDPS_POLICY_PICKER_VDIFF_TIEBREAKER_H
#define MDPS_POLICY_PICKER_VDIFF_TIEBREAKER_H

#include "probfd/engine_interfaces/heuristic_search_connector.h"
#include "probfd/engine_interfaces/policy_picker.h"

namespace probfd {

/// Namespace dedicated to policy tiebreaker implementations.
namespace policy_tiebreaking {

class VDiffTiebreaker : public TaskPolicyPicker {
    engine_interfaces::HeuristicSearchConnector* connector_;
    const value_type::value_t favor_large_gaps_;

public:
    VDiffTiebreaker(
        engine_interfaces::HeuristicSearchConnector* connector,
        value_type::value_t favor_large_gaps_);

protected:
    virtual int pick(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<OperatorID>& action_choices,
        const std::vector<Distribution<StateID>>& successors) override;
};

} // namespace policy_tiebreaking
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__