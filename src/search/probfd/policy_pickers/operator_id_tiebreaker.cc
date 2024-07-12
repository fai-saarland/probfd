#include "probfd/policy_pickers/operator_id_tiebreaker.h"

#include "probfd/transition.h"

#include "downward/operator_id.h"

#include <limits>

namespace probfd::policy_pickers {

OperatorIdTiebreaker::OperatorIdTiebreaker(
    bool stable_policy,
    bool prefer_smaller)
    : OperatorIdTiebreaker::StablePolicyPicker(stable_policy)
    , ascending_(prefer_smaller ? 1 : -1)
{
}

int OperatorIdTiebreaker::pick_index(
    FDRMDP&,
    StateID,
    std::optional<OperatorID>,
    const std::vector<Transition<OperatorID>>& greedy_transitions,
    algorithms::StateProperties&)
{
    int min_id = std::numeric_limits<int>::max();
    unsigned min_idx = std::numeric_limits<unsigned int>::max();
    for (int i = greedy_transitions.size() - 1; i >= 0; i--) {
        int id = greedy_transitions[i].action.get_index() * ascending_;
        if (id < min_id) {
            min_id = id;
            min_idx = i;
        }
    }
    return min_idx;
}

} // namespace probfd::policy_pickers
