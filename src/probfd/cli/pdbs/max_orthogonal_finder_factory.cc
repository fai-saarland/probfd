#include "probfd/cli/pdbs/max_orthogonal_finder_factory.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/pdbs/max_orthogonal_finder_factory.h"

using namespace downward;

using namespace probfd::pdbs;

using namespace language::plugins;

namespace {

InternalFunctionDefinitionBase&
add_additive_max_orthogonality_factory_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "additive_max_orthogonality_factory",
        &construct_shared<
            SubCollectionFinderFactory,
            AdditiveMaxOrthogonalityFinderFactory>);

    return f;
}

InternalFunctionDefinitionBase&
add_multiplicative_max_orthogonality_factory_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "multiplicative_max_orthogonality_factory",
        &construct_shared<
            SubCollectionFinderFactory,
            AdditiveMaxOrthogonalityFinderFactory>);

    return f;
}

} // namespace

namespace probfd::cli::pdbs {

void add_max_orthogonal_finder_factory_feature(Namespace& nspace)
{
    add_additive_max_orthogonality_factory_to_namespace(nspace);
    add_multiplicative_max_orthogonality_factory_to_namespace(nspace);
}

} // namespace probfd::cli::pdbs
