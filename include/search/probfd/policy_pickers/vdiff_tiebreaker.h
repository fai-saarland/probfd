#ifndef PROBFD_POLICY_PICKER_VDIFF_TIEBREAKER_H
#define PROBFD_POLICY_PICKER_VDIFF_TIEBREAKER_H

#include "probfd/policy_pickers/stable_policy_picker.h"

#include "probfd/fdr_types.h"

#include "downward/operator_id.h"

namespace plugins {
class Options;
} // namespace plugins

namespace probfd {
namespace policy_pickers {

class VDiffTiebreaker : public TaskStablePolicyPicker<VDiffTiebreaker> {
    const value_t favor_large_gaps_;

public:
    explicit VDiffTiebreaker(const plugins::Options&);
    explicit VDiffTiebreaker(bool stable_policy, value_t favor_large_gaps_);

    int pick_index(
        FDRMDP& mdp,
        StateID state,
        std::optional<OperatorID> prev_policy,
        const std::vector<Transition<OperatorID>>& greedy_transitions,
        engines::StateProperties& properties);
};

} // namespace policy_pickers
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__