#include "probfd/cli/merge_and_shrink/shrink_strategy_category.h"

#include "language/ast/internal_type_declaration.h"

#include "probfd/merge_and_shrink/shrink_strategy.h"

using namespace language::parser;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<ShrinkStrategy>(
        nspace,
        "PShrinkStrategy",
        "This page describes the various shrink strategies supported "
        "by the planner.");
}

} // namespace probfd::cli::merge_and_shrink
