#include "downward/cli/merge_and_shrink/merge_strategy_factory_category.h"

#include "downward/merge_and_shrink/merge_strategy_factory.h"

#include "language/ast/internal_type_declaration.h"

using namespace downward::merge_and_shrink;

using namespace language::parser;

namespace downward::cli::merge_and_shrink {

void add_merge_strategy_factory_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<MergeStrategyFactory>(
        nspace,
        "MergeStrategy",
        "This page describes the various merge strategies supported "
        "by the planner.");
}

} // namespace downward::cli::merge_and_shrink
