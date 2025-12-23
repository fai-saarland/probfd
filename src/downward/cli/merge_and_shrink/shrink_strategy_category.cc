#include "downward/cli/merge_and_shrink/shrink_strategy_category.h"

#include "language/plugins/registry.h"

#include "downward/merge_and_shrink/shrink_strategy.h"

using namespace downward::merge_and_shrink;

using namespace language::plugins;

namespace downward::cli::merge_and_shrink {

void add_shrink_strategy_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<ShrinkStrategy>(
        "ShrinkStrategy",
        "This page describes the various shrink strategies supported "
        "by the planner.");
}

} // namespace downward::cli::merge_and_shrink
