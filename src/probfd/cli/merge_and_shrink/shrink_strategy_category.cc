#include "probfd/cli/merge_and_shrink/shrink_strategy_category.h"

#include "language/plugins/registry.h"

#include "probfd/merge_and_shrink/shrink_strategy.h"

using namespace language::plugins;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<ShrinkStrategy>(
        "PShrinkStrategy",
        "This page describes the various shrink strategies supported "
        "by the planner.");
}

} // namespace probfd::cli::merge_and_shrink
