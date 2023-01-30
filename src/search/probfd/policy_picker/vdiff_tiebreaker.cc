#include "probfd/policy_picker/vdiff_tiebreaker.h"

#include "probfd/engines/heuristic_search_state_information.h"

namespace probfd {
namespace policy_tiebreaking {

VDiffTiebreaker::VDiffTiebreaker(
    engine_interfaces::HeuristicSearchConnector* connector,
    value_type::value_t favor_large_gaps)
    : connector_(connector)
    , favor_large_gaps_(favor_large_gaps)
{
}

int VDiffTiebreaker::pick(
    const StateID&,
    const ActionID&,
    const std::vector<OperatorID>&,
    const std::vector<Distribution<StateID>>& successors)
{
    value_type::value_t best = value_type::inf;
    unsigned choice = 1;
    for (int i = successors.size() - 1; i >= 0; --i) {
        const Distribution<StateID>& t = successors[i];
        value_type::value_t sum = value_type::zero;
        for (auto it = t.begin(); it != t.end(); ++it) {
            const auto& value = connector_->lookup_dual_bounds(it->element);
            sum += it->probability * value->error_bound();
        }
        if (value_type::approx_less()(favor_large_gaps_ * sum, best)) {
            best = sum;
            choice = i;
        }
    }
    return choice;
}

} // namespace policy_tiebreaking
} // namespace probfd
