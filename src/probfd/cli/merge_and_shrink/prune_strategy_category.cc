#include "probfd/cli/merge_and_shrink/prune_strategy_category.h"

#include "language/ast/internal_type_declaration.h"

#include "probfd/merge_and_shrink/prune_strategy.h"

using namespace probfd::merge_and_shrink;
using namespace language::parser;

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<PruneStrategy>(
        nspace,
        "PruneStrategy",
        "This page describes the various pruning strategies "
        "supported by the planner.");
}

} // namespace probfd::cli::merge_and_shrink
