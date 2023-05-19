#include "probfd/transition_sorters/vdiff_sorter.h"

#include "probfd/engine_interfaces/heuristic_search_interface.h"

#include <algorithm>
#include <ranges>

namespace probfd {
namespace transition_sorters {

VDiffSorter::VDiffSorter(const value_t favor_large_gaps)
    : favor_large_gaps_(favor_large_gaps)
{
}

void VDiffSorter::sort(
    StateID,
    const std::vector<OperatorID>&,
    std::vector<Distribution<StateID>>& all_successors,
    engine_interfaces::HeuristicSearchInterface& hsi)
{
    std::vector<double> k0;
    k0.reserve(all_successors.size());
    for (const auto& successor_dist : all_successors) {
        k0.emplace_back(successor_dist.expectation([=, &hsi](StateID succ) {
            return favor_large_gaps_ * hsi.lookup_bounds(succ).length();
        }));
    }

    std::ranges::sort(
        std::views::zip(all_successors, k0),
        [](const auto& left, const auto& right) {
            return std::get<1>(left) < std::get<1>(right);
        });
}

} // namespace transition_sorters
} // namespace probfd
