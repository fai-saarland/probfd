#include "probfd/transition_sorters/vdiff_sorter.h"

#include "downward/plugins/plugin.h"

namespace probfd::transition_sorters {

static class FDRTransitionSorterCategoryPlugin
    : public plugins::TypedCategoryPlugin<FDRTransitionSorter> {
public:
    FDRTransitionSorterCategoryPlugin()
        : TypedCategoryPlugin("FDRTransitionSorter")
    {
    }
} _category_plugin_collection;

class VDiffSorterFeature
    : public plugins::TypedFeature<FDRTransitionSorter, VDiffSorter> {
public:
    VDiffSorterFeature()
        : TypedFeature("value_gap_sort")
    {
        add_option<bool>("prefer_large_gaps", "", "false");
    }

    std::shared_ptr<VDiffSorter>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return std::make_shared<VDiffSorter>(
            opts.get<bool>("prefer_large_gaps"));
    }
};

static plugins::FeaturePlugin<VDiffSorterFeature> _plugin_value_gap;

} // namespace probfd::transition_sorters
