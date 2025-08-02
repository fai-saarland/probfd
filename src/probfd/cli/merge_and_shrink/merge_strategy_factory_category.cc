#include "probfd/cli/merge_and_shrink/merge_strategy_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/merge_and_shrink/merge_strategy_factory.h"

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
};
}

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<MergeStrategyFactoryCategoryPlugin>();
}

} // namespace
