#include "probfd/merge_and_shrink/pruning_strategy.h"

#include "downward/plugins/plugin.h"

namespace probfd::merge_and_shrink {

static class PruningStrategyCategoryPlugin
    : public plugins::TypedCategoryPlugin<PruningStrategy> {
public:
    PruningStrategyCategoryPlugin()
        : TypedCategoryPlugin("PruningStrategy")
    {
        document_synopsis("This page describes the various pruning strategies "
                          "supported by the planner.");
    }
} _category_plugin;

} // namespace probfd::merge_and_shrink
