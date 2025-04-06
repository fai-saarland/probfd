#ifndef PROBFD_POLICY_PICKERS_OPERATOR_ID_TIEBREAKER_H
#define PROBFD_POLICY_PICKERS_OPERATOR_ID_TIEBREAKER_H

#include "probfd/policy_pickers/stable_policy_picker.h"

#include "probfd/fdr_types.h"

#include "downward/operator_id.h"

#include <optional>
#include <vector>

namespace probfd::policy_pickers {

class OperatorIdTiebreaker
    : public StablePolicyPicker<
          downward::State,
          downward::OperatorID,
          OperatorIdTiebreaker> {
    const int ascending_;

public:
    explicit OperatorIdTiebreaker(bool stable_policy, bool prefer_smaller);

    int pick_index(
        FDRMDP& mdp,
        std::optional<downward::OperatorID> prev_policy,
        const std::vector<TransitionTail<downward::OperatorID>>&
            greedy_transitions,
        algorithms::StateProperties& properties) override;
};

} // namespace probfd::policy_pickers

#endif // PROBFD_POLICY_PICKERS_OPERATOR_ID_TIEBREAKER_H