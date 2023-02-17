#include "probfd/policy_pickers/vdiff_tiebreaker.h"

#include "probfd/engine_interfaces/heuristic_search_interface.h"

namespace probfd {
namespace policy_pickers {

VDiffTiebreaker::VDiffTiebreaker(value_t favor_large_gaps)
    : favor_large_gaps_(favor_large_gaps)
{
}

int VDiffTiebreaker::pick(
    StateID,
    ActionID,
    const std::vector<OperatorID>&,
    const std::vector<Distribution<StateID>>& successors,
    engine_interfaces::HeuristicSearchInterface& hs_interface)
{
    value_t best = INFINITE_VALUE;
    unsigned choice = 1;
    for (int i = successors.size() - 1; i >= 0; --i) {
        const Distribution<StateID>& t = successors[i];
        value_t sum = 0_vt;
        for (auto it = t.begin(); it != t.end(); ++it) {
            auto value = hs_interface.lookup_dual_bounds(it->item);
            sum += it->probability * value.length();
        }
        if (is_approx_less(favor_large_gaps_ * sum, best)) {
            best = sum;
            choice = i;
        }
    }
    return choice;
}

} // namespace policy_pickers
} // namespace probfd
