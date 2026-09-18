#include "probfd/transition_sorters/vdiff_sorter.h"

#include "probfd/algorithms/state_properties.h"

#include "probfd/distribution.h"
#include "probfd/interval.h"
#include "probfd/labelled_successor_distribution.h"

#include <algorithm>
#include <functional>
#include <ranges>
#include <utility>

using namespace downward;

namespace probfd::transition_sorters {

VDiffSorter::VDiffSorter(value_t favor_large_gaps)
    : favor_large_gaps_(favor_large_gaps)
{
}

void VDiffSorter::sort(
    const State&,
    const std::vector<OperatorID>&,
    std::vector<SuccessorDistribution>& successor_dists,
    algorithms::StateProperties& properties)
{
    auto rv = [this, &properties](StateID succ) {
        return favor_large_gaps_ * properties.lookup_bounds(succ).length();
    };

    std::vector<double> k0;
    k0.reserve(successor_dists.size());

    for (const auto& [non_source_successor_dist, non_source_probability] :
         successor_dists) {
        k0.emplace_back(
            non_source_successor_dist.expectation(rv) / non_source_probability);
    }

    std::ranges::sort(
        std::views::zip(successor_dists, k0),
        {},
        [](const auto& p) { return std::get<1>(p); });
}

} // namespace probfd::transition_sorters
