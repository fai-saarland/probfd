#ifndef PROBFD_POLICY_PICKER_OPERATOR_ID_TIEBREAKER_H
#define PROBFD_POLICY_PICKER_OPERATOR_ID_TIEBREAKER_H

#include "probfd/policy_pickers/stable_policy_picker.h"

#include "probfd/fdr_types.h"

#include "downward/operator_id.h"

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
        FDRMDP& mdp,
        StateID state,
        std::optional<OperatorID> prev_policy,
        const std::vector<Transition<OperatorID>>& greedy_transitions,
        engine_interfaces::StateProperties& properties);
};

} // namespace policy_pickers
} // namespace probfd

#endif // __OPERATOR_ID_TIEBREAKER_H__