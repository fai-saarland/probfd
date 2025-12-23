#include "probfd/cli/pdbs/fully_additive_finder_factory.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/pdbs/fully_additive_finder_factory.h"

using namespace downward;

using namespace probfd::pdbs;

using namespace language::plugins;

namespace probfd::cli::pdbs {

InternalFunctionDefinitionBase&
add_fully_additive_finder_factory_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "fully_additive_factory",
        &construct_shared<
            SubCollectionFinderFactory,
            FullyAdditiveFinderFactory>);

    return f;
}

} // namespace probfd::cli::pdbs
