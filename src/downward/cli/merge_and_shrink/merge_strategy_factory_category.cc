#include "downward/cli/merge_and_shrink/merge_strategy_factory_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/merge_and_shrink/merge_strategy_factory.h"

using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

namespace downward::cli::merge_and_shrink {

void add_merge_strategy_factory_category(RawRegistry& raw_registry)
{
    auto& category = raw_registry.insert_category_plugin<MergeStrategyFactory>(
        "MergeStrategy");
    category.document_synopsis(
        "This page describes the various merge strategies supported "
        "by the planner.");
}

} // namespace downward::cli::merge_and_shrink
