#include "downward_plugins/plugins/plugin.h"

#include "downward/merge_and_shrink/merge_strategy_factory.h"

using namespace std;
using namespace merge_and_shrink;

using namespace downward_plugins::plugins;

namespace {

class MergeStrategyFactoryCategoryPlugin
    : public TypedCategoryPlugin<MergeStrategyFactory> {
public:
    MergeStrategyFactoryCategoryPlugin()
        : TypedCategoryPlugin("MergeStrategy")
    {
        document_synopsis(
            "This page describes the various merge strategies supported "
            "by the planner.");
    }
} _category_plugin;

} // namespace
