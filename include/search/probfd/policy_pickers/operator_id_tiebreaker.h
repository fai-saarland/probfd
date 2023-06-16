#ifndef PROBFD_POLICY_PICKER_OPERATOR_ID_TIEBREAKER_H
#define PROBFD_POLICY_PICKER_OPERATOR_ID_TIEBREAKER_H

#include "probfd/policy_pickers/stable_policy_picker.h"

#include "operator_id.h"

namespace plugins {
class Options;
} // namespace plugins

namespace probfd {
namespace policy_pickers {

class OperatorIdTiebreaker
    : public TaskStablePolicyPicker<OperatorIdTiebreaker> {
    const int ascending_;

public:
    explicit OperatorIdTiebreaker(const plugins::Options&);
    explicit OperatorIdTiebreaker(bool stable_policy, int ascending);

    int pick_index(
        engine_interfaces::StateSpace<State, OperatorID>& state_space,
        StateID state,
        std::optional<OperatorID> prev_policy,
        const std::vector<OperatorID>& action_choices,
        const std::vector<Distribution<StateID>>& successors,
        engine_interfaces::StateProperties& properties);
};

} // namespace policy_pickers
} // namespace probfd

#endif // __OPERATOR_ID_TIEBREAKER_H__