#include "probfd/transition_sorters/vdiff_sorter.h"

#include "probfd/algorithms/state_properties.h"

#include "probfd/interval.h"
#include "probfd/labelled_successor_pmf.h"
#include "probfd/pmf.h"

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
    std::vector<SuccessorPMF>& successor_pmfs,
    algorithms::StateProperties& properties)
{
    auto rv = [this, &properties](StateID succ) {
        return favor_large_gaps_ * properties.lookup_bounds(succ).length();
    };

    std::vector<double> k0;
    k0.reserve(successor_pmfs.size());

    for (const auto& [non_source_successor_pmf, non_source_probability] :
         successor_pmfs) {
        k0.emplace_back(
            non_source_successor_pmf.expectation(rv) / non_source_probability);
    }

    std::ranges::sort(
        std::views::zip(successor_pmfs, k0),
        {},
        [](const auto& p) { return std::get<1>(p); });
}

} // namespace probfd::transition_sorters
