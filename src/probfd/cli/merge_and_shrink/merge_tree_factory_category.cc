#include "probfd/cli/merge_and_shrink/merge_tree_factory_category.h"

#include "probfd/merge_and_shrink/merge_tree_factory.h"

#include "probfd/merge_and_shrink/merge_tree.h"

#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_type_declaration.h"

using namespace language::parser;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_merge_tree_factory_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<MergeTreeFactory>(
        nspace,
        "PMergeTree",
        "This page describes the available merge trees that can be used to "
        "precompute a merge strategy, either for the entire task or a "
        "given "
        "subset of transition systems of a given factored transition "
        "system.\n"
        "Merge trees are typically used in the merge strategy of type "
        "'precomputed', but they can also be used as fallback merge "
        "strategies in "
        "'combined' merge strategies.");

    insert_enum_declaration<UpdateOption>(
        nspace,
        "UpdateOption",
        {{"use_first",
          "the node representing the index that would have been merged earlier "
          "survives"},
         {"use_second",
          "the node representing the index that would have been merged later "
          "survives"},
         {"use_random", "a random node (of the above two) survives"}});
}

} // namespace probfd::cli::merge_and_shrink
