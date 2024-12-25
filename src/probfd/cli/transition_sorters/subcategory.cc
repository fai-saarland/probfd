#include "downward/cli/plugins/plugin.h"

#include "probfd/transition_sorters/vdiff_sorter.h"

using namespace utils;

using namespace probfd;
using namespace probfd::transition_sorters;

using namespace downward::cli::plugins;

namespace {

class FDRTransitionSorterCategoryPlugin
    : public TypedCategoryPlugin<FDRTransitionSorter> {
public:
    FDRTransitionSorterCategoryPlugin()
        : TypedCategoryPlugin("FDRTransitionSorter")
    {
    }
} _category_plugin_collection;

class VDiffSorterFeature
    : public TypedFeature<FDRTransitionSorter, VDiffSorter> {
public:
    VDiffSorterFeature()
        : TypedFeature("value_gap_sort")
    {
        add_option<bool>("prefer_large_gaps", "", "false");
    }

    std::shared_ptr<VDiffSorter>
    create_component(const Options& opts, const Context&) const override
    {
        return std::make_shared<VDiffSorter>(
            opts.get<bool>("prefer_large_gaps"));
    }
};

FeaturePlugin<VDiffSorterFeature> _plugin_value_gap;

} // namespace
