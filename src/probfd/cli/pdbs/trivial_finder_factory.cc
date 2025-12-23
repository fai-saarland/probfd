#include "probfd/cli/pdbs/trivial_finder_factory.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/pdbs/trivial_finder_factory.h"

using namespace downward;

using namespace probfd::pdbs;

using namespace language::plugins;

namespace probfd::cli::pdbs {

InternalFunctionDefinitionBase&
add_trivial_finder_factory_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "finder_trivial_factory",
        &construct_shared<SubCollectionFinderFactory, TrivialFinderFactory>);

    return f;
}

} // namespace probfd::cli::pdbs
