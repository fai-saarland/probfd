#include "probfd/cli/cartesian_abstractions/trace_based_flaw_generator.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/cartesian_abstractions/trace_based_flaw_generator.h"

using namespace downward;
using namespace utils;

using namespace probfd::cartesian_abstractions;

using namespace language::parser;

namespace probfd::cli::cartesian_abstractions {

InternalFunctionDefinitionBase&
add_astar_flaw_generator_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<
        &construct_shared<FlawGeneratorFactory, AStarFlawGeneratorFactory>>(
        nspace,
        "flaws_astar");

    return f;
}

} // namespace probfd::cli::cartesian_abstractions
