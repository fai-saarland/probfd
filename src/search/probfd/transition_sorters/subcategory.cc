#include "probfd/transition_sorters/vdiff_sorter_factory.h"

#include "plugins/plugin.h"

namespace probfd {
namespace transition_sorters {

static class TaskTransitionSorterFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<TaskTransitionSorterFactory> {
public:
    TaskTransitionSorterFactoryCategoryPlugin()
        : TypedCategoryPlugin("TaskTransitionSorterFactory")
    {
    }
} _category_plugin_collection;

class VDiffSorterFactoryFeature
    : public plugins::
          TypedFeature<TaskTransitionSorterFactory, VDiffSorterFactory> {
public:
    VDiffSorterFactoryFeature()
        : TypedFeature("value_gap_sort_factory")
    {
        add_option<bool>("prefer_large_gaps", "", "false");
    }
};

static plugins::FeaturePlugin<VDiffSorterFactoryFeature> _plugin_value_gap;

} // namespace transition_sorters
} // namespace probfd