#include "probfd/cli/pdbs/fully_additive_finder_factory.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/pdbs/fully_additive_finder_factory.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {

InternalFunctionDefinitionBase& add_fully_additive_factory_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "fully_additive_factory",
        &downward::cli::plugins::construct_shared<
            SubCollectionFinderFactory,
            FullyAdditiveFinderFactory>);

    return f;
}

} // namespace

namespace probfd::cli::pdbs {

void add_fully_additive_finder_factory_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_fully_additive_factory_to_namespace(n);
}

} // namespace probfd::cli::pdbs
