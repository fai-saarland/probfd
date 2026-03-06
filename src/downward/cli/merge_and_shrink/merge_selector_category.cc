#include "downward/cli/merge_and_shrink/merge_selector_category.h"

#include "downward/merge_and_shrink/merge_selector.h"

#include "language/ast/internal_type_declaration.h"

using namespace downward::merge_and_shrink;

using namespace language::parser;

namespace downward::cli::merge_and_shrink {

void add_merge_selector_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<MergeSelector>(
        nspace,
        "MergeSelector",
        "This page describes the available merge selectors. They are used "
        "to "
        "compute the next merge purely based on the state of the given "
        "factored "
        "transition system. They are used in the merge strategy of type "
        "'stateless', but they can also easily be used in different "
        "'combined' "
        "merged strategies.");
}

} // namespace downward::cli::merge_and_shrink
