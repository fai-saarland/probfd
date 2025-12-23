#include "probfd/cli/heuristics/constant_heuristic.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/heuristics/constant_heuristic.h"

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace language::plugins;

namespace probfd::cli::heuristics {

InternalFunctionDefinitionBase&
add_blind_heuristic_factory_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "blind_heuristic",
        &language::plugins::
            construct_shared<TaskHeuristicFactory, BlindHeuristicFactory>);

    f.document_title("Blind Heuristic");
    f.document_synopsis(
        "This heuristic always returns an estimate of 0 for every state.");

    return f;
}

} // namespace probfd::cli::heuristics
