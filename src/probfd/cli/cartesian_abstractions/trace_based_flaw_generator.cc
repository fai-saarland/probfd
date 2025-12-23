#include "probfd/cli/cartesian_abstractions/trace_based_flaw_generator.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/cartesian_abstractions/trace_based_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace probfd::cartesian_abstractions;

using namespace language::plugins;

namespace probfd::cli::cartesian_abstractions {

InternalFunctionDefinitionBase&
add_astar_flaw_generator_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "flaws_astar",
        &construct_shared<FlawGeneratorFactory, AStarFlawGeneratorFactory>);

    return f;
}

} // namespace probfd::cli::cartesian_abstractions
