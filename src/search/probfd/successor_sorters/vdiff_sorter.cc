#include "probfd/successor_sorters/vdiff_sorter.h"

#include "probfd/engine_interfaces/heuristic_search_interface.h"

#include <algorithm>

namespace probfd {
namespace successor_sorters {

VDiffSorter::VDiffSorter(const value_t favor_large_gaps)
    : favor_large_gaps_(favor_large_gaps)
{
}

void VDiffSorter::sort(
    StateID,
    const std::vector<OperatorID>&,
    std::vector<Distribution<StateID>>& all_successors,
    engine_interfaces::HeuristicSearchInterface& hs_interface)
{
    std::vector<std::pair<double, unsigned>> k0;
    k0.reserve(all_successors.size());
    for (const auto& successor_dist : all_successors) {
        value_t sum = 0;
        for (const auto [succ, prob] : successor_dist) {
            sum += prob * favor_large_gaps_ *
                   hs_interface.lookup_dual_bounds(succ).length();
        }
        k0.emplace_back(sum, k0.size());
    }

    // Would be less redundant with std::ranges::zip (C++ 23) ...
    std::ranges::sort(k0);
    std::vector<Distribution<StateID>> res;
    res.reserve(all_successors.size());
    for (unsigned i = 0; i < k0.size(); ++i) {
        res.push_back(std::move(all_successors[k0[i].second]));
    }
    res.swap(all_successors);
}

} // namespace successor_sorting
} // namespace probfd
