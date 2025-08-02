#include "downward/cli/merge_and_shrink/shrink_strategy_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/merge_and_shrink/shrink_strategy.h"

using namespace std;
using namespace downward::merge_and_shrink;

using namespace downward::cli::plugins;

namespace {
class ShrinkStrategyCategoryPlugin
    : public TypedCategoryPlugin<ShrinkStrategy> {
public:
    ShrinkStrategyCategoryPlugin()
        : TypedCategoryPlugin("ShrinkStrategy")
    {
        document_synopsis(
            "This page describes the various shrink strategies supported "
            "by the planner.");
    }
};
}

namespace downward::cli::merge_and_shrink {

void add_shrink_strategy_category(RawRegistry& raw_registry)
{
    raw_registry.insert_category_plugin<ShrinkStrategyCategoryPlugin>();
}

} // namespace
