#include "probfd/cli/merge_and_shrink/merge_strategy_factory_category.h"

#include "language/ast/internal_function_definition.h"
#include "language/ast/internal_type_declaration.h"

#include "probfd/merge_and_shrink/merge_strategy_factory.h"

using namespace language::parser;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<MergeStrategyFactory>(
        nspace,
        "PMergeStrategy",
        "This page describes the various merge strategies supported "
        "by the planner.");
}

} // namespace probfd::cli::merge_and_shrink
