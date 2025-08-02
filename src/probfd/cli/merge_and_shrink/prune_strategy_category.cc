#include "probfd/cli/merge_and_shrink/prune_strategy_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/merge_and_shrink/prune_strategy.h"

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
};
}

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<PruneStrategyCategoryPlugin>();
}


} // namespace probfd::merge_and_shrink
