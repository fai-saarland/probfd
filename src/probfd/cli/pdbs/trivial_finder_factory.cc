#include "probfd/cli/pdbs/trivial_finder_factory.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/pdbs/trivial_finder_factory.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {

InternalFunctionDefinitionBase& add_trivial_finder_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "finder_trivial_factory",
        &downward::cli::plugins::
            construct_shared<SubCollectionFinderFactory, TrivialFinderFactory>);

    return f;
}

} // namespace

namespace probfd::cli::pdbs {

void add_trivial_finder_factory_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_trivial_finder_to_namespace(n);
}

} // namespace probfd::cli::pdbs
