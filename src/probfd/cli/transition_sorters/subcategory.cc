#include "probfd/cli/transition_sorters/subcategory.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/transition_sorters/vdiff_sorter.h"

using namespace downward;
using namespace downward::utils;

using namespace probfd;
using namespace probfd::transition_sorters;

using namespace downward::cli::plugins;

namespace {

class VDiffSorterFeature
    : public SharedTypedFeature<FDRTransitionSorter> {
public:
    VDiffSorterFeature()
        : SharedTypedFeature("value_gap_sort")
    {
        add_option<bool>("prefer_large_gaps", "", "false");
    }

    std::shared_ptr<FDRTransitionSorter>
    create_component(const Options& opts, const Context&) const override
    {
        return std::make_shared<VDiffSorter>(
            opts.get<bool>("prefer_large_gaps"));
    }
};
}

namespace probfd::cli::transiton_sorters {

void add_transition_sorter_features(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<FDRTransitionSorter>("FDRTransitionSorter");
    raw_registry.insert_feature_plugin<VDiffSorterFeature>();
}

} // namespace
