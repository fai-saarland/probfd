#include "probfd/cli/merge_and_shrink/merge_strategy_factory_category.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/merge_and_shrink/merge_strategy_factory.h"

using namespace language::plugins;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<MergeStrategyFactory>(
        "PMergeStrategy",
        "This page describes the various merge strategies supported "
        "by the planner.");
}

} // namespace probfd::cli::merge_and_shrink
