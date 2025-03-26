#include "probfd/merge_and_shrink/prune_strategy.h"

#include "downward/cli/plugins/plugin.h"

using namespace probfd::merge_and_shrink;
using namespace downward::cli::plugins;

namespace {

class PruneStrategyCategoryPlugin
    : public TypedCategoryPlugin<PruneStrategy> {
public:
    PruneStrategyCategoryPlugin()
        : TypedCategoryPlugin("PruneStrategy")
    {
        document_synopsis("This page describes the various pruning strategies "
                          "supported by the planner.");
    }
} _category_plugin;

} // namespace probfd::merge_and_shrink
