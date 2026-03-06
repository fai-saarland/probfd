#include "probfd/cli/transition_sorters/subcategory.h"

#include "language/ast/internal_function_definition.h"
#include "language/ast/internal_type_declaration.h"

#include "probfd/transition_sorters/vdiff_sorter.h"

using namespace downward;

using namespace probfd;
using namespace probfd::transition_sorters;

using namespace language::parser;

namespace {

InternalFunctionDefinitionBase&
add_vdiff_sorter_to_namespace(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<
        &language::parser::
            construct_shared<FDRTransitionSorter, VDiffSorter, bool>>(
        nspace,
        "value_gap_sort");
    f.make_optional_argument_with_default(0, "prefer_large_gaps", "false");

    return f;
}

} // namespace

namespace probfd::cli::transiton_sorters {

void add_transition_sorter_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<FDRTransitionSorter>(
        nspace,
        "FDRTransitionSorter",
        "");
}

void add_transition_sorter_features(NamespaceLevelDeclarationList& nspace)
{
    add_vdiff_sorter_to_namespace(nspace);
}

} // namespace probfd::cli::transiton_sorters
