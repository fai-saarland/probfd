#include "downward/cli/plugins/plugin.h"

#include "downward/merge_and_shrink/shrink_strategy.h"

using namespace std;
using namespace merge_and_shrink;

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
} _category_plugin;

} // namespace
