#include "probfd/cli/cartesian_abstractions/trace_based_flaw_generator.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/cartesian_abstractions/trace_based_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace probfd::cartesian_abstractions;

using namespace downward::cli::plugins;

namespace {

InternalFunctionDefinitionBase& add_astar_flaw_generator_astar_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "flaws_astar",
        &cli::plugins::
            construct_shared<FlawGeneratorFactory, AStarFlawGeneratorFactory>);

    return f;
}

} // namespace

namespace probfd::cli::cartesian_abstractions {

void add_astar_flaw_generator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_astar_flaw_generator_astar_to_namespace(n);
}

} // namespace probfd::cli::cartesian_abstractions
