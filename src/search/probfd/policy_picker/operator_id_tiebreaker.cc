#include "probfd/policy_picker/operator_id_tiebreaker.h"

#include "operator_id.h"

#include <limits>

namespace probfd {
namespace policy_tiebreaking {

OperatorIdTiebreaker::OperatorIdTiebreaker(int ascending)
    : ascending_(ascending)
{
}

int OperatorIdTiebreaker::pick(
    const StateID&,
    const ActionID&,
    const std::vector<OperatorID>& choices,
    const std::vector<Distribution<StateID>>&,
    engine_interfaces::HeuristicSearchInterface&)
{
    int min_id = std::numeric_limits<int>::max();
    unsigned min_idx = -1;
    for (int i = choices.size() - 1; i >= 0; i--) {
        int id = choices[i].get_index() * ascending_;
        if (id < min_id) {
            min_id = id;
            min_idx = i;
        }
    }
    return min_idx;
}

} // namespace policy_tiebreaking
} // namespace probfd
