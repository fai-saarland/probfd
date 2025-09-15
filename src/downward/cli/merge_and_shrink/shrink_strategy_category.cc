#include "downward/cli/merge_and_shrink/shrink_strategy_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/merge_and_shrink/shrink_strategy.h"

using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

namespace downward::cli::merge_and_shrink {

void add_shrink_strategy_category(RawRegistry& raw_registry)
{
    auto& category =
        raw_registry.insert_category_plugin<ShrinkStrategy>("ShrinkStrategy");
    category.document_synopsis(
        "This page describes the various shrink strategies supported "
        "by the planner.");
}

} // namespace downward::cli::merge_and_shrink
