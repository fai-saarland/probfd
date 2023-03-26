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
    std::vector<Distribution<StateID>>& successors,
    engine_interfaces::HeuristicSearchInterface& hs_interface)
{
    std::vector<std::pair<double, unsigned>> k0;
    k0.reserve(successors.size());
    for (int i = successors.size() - 1; i >= 0; --i) {
        const auto& t = successors[i].data();
        value_t sum = 0;
        for (const auto [succ, prob] : t) {
            const value_t gap = favor_large_gaps_ *
                                hs_interface.lookup_dual_bounds(succ).length();
            sum += prob * gap;
        }
        k0.emplace_back(sum, i);
    }

    // Would be less redundant with std::ranges::zip (C++ 23) ...
    std::ranges::sort(k0);
    std::vector<Distribution<StateID>> res;
    res.reserve(successors.size());
    for (unsigned i = 0; i < k0.size(); ++i) {
        res.push_back(std::move(successors[k0[i].second]));
    }
    res.swap(successors);
}

} // namespace successor_sorting
} // namespace probfd
