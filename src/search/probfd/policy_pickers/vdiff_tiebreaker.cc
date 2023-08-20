#include "probfd/policy_pickers/vdiff_tiebreaker.h"

#include "probfd/algorithms/state_properties.h"

#include "downward/plugins/options.h"

namespace probfd {
namespace policy_pickers {

VDiffTiebreaker::VDiffTiebreaker(const plugins::Options& opts)
    : VDiffTiebreaker(
          opts.get<bool>("stable_policy"),
          opts.get<bool>("prefer_large_gaps") ? -1 : 1)
{
}

VDiffTiebreaker::VDiffTiebreaker(bool stable_policy, value_t favor_large_gaps)
    : TaskStablePolicyPicker<VDiffTiebreaker>(stable_policy)
    , favor_large_gaps_(favor_large_gaps)
{
}

int VDiffTiebreaker::pick_index(
    FDRMDP&,
    StateID,
    std::optional<OperatorID>,
    const std::vector<Transition<OperatorID>>& greedy_transitions,
    algorithms::StateProperties& properties)
{
    auto it = std::ranges::min_element(
        greedy_transitions,
        [](value_t lhs, value_t rhs) { return is_approx_less(lhs, rhs); },
        [&, factor = favor_large_gaps_](const Transition<OperatorID>& t) {
            return t.successor_dist.expectation([&](StateID id) {
                return factor * properties.lookup_bounds(id).length();
            });
        });

    return std::distance(greedy_transitions.begin(), it);
}

} // namespace policy_pickers
} // namespace probfd
