#include "probfd/transition_sorters/vdiff_sorter_factory.h"
#include "probfd/transition_sorters/vdiff_sorter.h"

#include "downward/plugins/options.h"

namespace probfd {
namespace transition_sorters {

VDiffSorterFactory::VDiffSorterFactory(const plugins::Options& opts)
    : favor_large_gaps_(opts.get<bool>("prefer_large_gaps") ? -1 : 1)
{
}

std::shared_ptr<FDRTransitionSorter>
VDiffSorterFactory::create_transition_sorter(FDRMDP*)
{
    return std::make_unique<VDiffSorter>(favor_large_gaps_);
}

} // namespace transition_sorters
} // namespace probfd
