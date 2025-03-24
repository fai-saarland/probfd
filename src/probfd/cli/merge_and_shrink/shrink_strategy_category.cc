#include "probfd/merge_and_shrink/shrink_strategy.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace {

class ShrinkStrategyCategoryPlugin
    : public TypedCategoryPlugin<ShrinkStrategy> {
public:
    ShrinkStrategyCategoryPlugin()
        : TypedCategoryPlugin("PShrinkStrategy")
    {
        document_synopsis(
            "This page describes the various shrink strategies supported "
            "by the planner.");
    }
} _category_plugin;

} // namespace
