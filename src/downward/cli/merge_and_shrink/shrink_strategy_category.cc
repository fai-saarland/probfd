#include "downward/cli/merge_and_shrink/shrink_strategy_category.h"

#include "downward/merge_and_shrink/shrink_strategy.h"

#include "language/ast/internal_type_declaration.h"

using namespace downward::merge_and_shrink;

using namespace language::parser;

namespace downward::cli::merge_and_shrink {

void add_shrink_strategy_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<ShrinkStrategy>(
        nspace,
        "ShrinkStrategy",
        "This page describes the various shrink strategies supported "
        "by the planner.");
}

} // namespace downward::cli::merge_and_shrink
