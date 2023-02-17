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
    std::vector<std::pair<double, unsigned>> k1;
    for (int i = successors.size() - 1; i >= 0; --i) {
        const auto& t = successors[i].data();
        value_t sum = 0;
        for (unsigned j = 0; j < t.size(); ++j) {
            const auto& suc = t[j];
            k1.emplace_back(
                std::abs(
                    favor_large_gaps_ *
                    hs_interface.lookup_dual_bounds(suc.item).length()),
                j);
            sum += suc.probability * k1.back().first;
        }
        k0.emplace_back(sum, i);
        std::ranges::sort(k1);
        std::vector<ItemProbabilityPair<StateID>> sor;
        sor.reserve(t.size());
        for (unsigned j = 0; j < k1.size(); ++j) {
            sor.push_back(t[k1[j].second]);
        }
        k1.clear();
    }
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
