#include "probfd/merge_and_shrink/merge_strategy_factory.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace {

class MergeStrategyFactoryCategoryPlugin
    : public TypedCategoryPlugin<MergeStrategyFactory> {
public:
    MergeStrategyFactoryCategoryPlugin()
        : TypedCategoryPlugin("PMergeStrategy")
    {
        document_synopsis(
            "This page describes the various merge strategies supported "
            "by the planner.");
    }
} _category_plugin;

} // namespace
