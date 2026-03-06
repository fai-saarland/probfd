#include "probfd/cli/pdbs/max_orthogonal_finder_factory.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/pdbs/max_orthogonal_finder_factory.h"

using namespace downward;

using namespace probfd::pdbs;

using namespace language::parser;

namespace {

InternalFunctionDefinitionBase&
add_additive_max_orthogonality_factory_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&construct_shared<
        SubCollectionFinderFactory,
        AdditiveMaxOrthogonalityFinderFactory>>(
        nspace,
        "additive_max_orthogonality_factory");

    return f;
}

InternalFunctionDefinitionBase&
add_multiplicative_max_orthogonality_factory_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&construct_shared<
        SubCollectionFinderFactory,
        AdditiveMaxOrthogonalityFinderFactory>>(
        nspace,
        "multiplicative_max_orthogonality_factory");

    return f;
}

} // namespace

namespace probfd::cli::pdbs {

void add_max_orthogonal_finder_factory_feature(
    NamespaceLevelDeclarationList& nspace)
{
    add_additive_max_orthogonality_factory_to_namespace(nspace);
    add_multiplicative_max_orthogonality_factory_to_namespace(nspace);
}

} // namespace probfd::cli::pdbs
