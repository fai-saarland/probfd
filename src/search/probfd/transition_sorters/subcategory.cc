#include "probfd/transition_sorters/vdiff_sorter_factory.h"

#include "downward/plugins/plugin.h"

namespace probfd::transition_sorters {

static class FDRTransitionSorterFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<FDRTransitionSorterFactory> {
public:
    FDRTransitionSorterFactoryCategoryPlugin()
        : TypedCategoryPlugin("FDRTransitionSorterFactory")
    {
    }
} _category_plugin_collection;

class VDiffSorterFactoryFeature
    : public plugins::
          TypedFeature<FDRTransitionSorterFactory, VDiffSorterFactory> {
public:
    VDiffSorterFactoryFeature()
        : TypedFeature("value_gap_sort_factory")
    {
        add_option<bool>("prefer_large_gaps", "", "false");
    }
};

static plugins::FeaturePlugin<VDiffSorterFactoryFeature> _plugin_value_gap;

} // namespace probfd::transition_sorters
