#include "probfd/cli/heuristics/constant_heuristic.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/heuristics/constant_heuristic.h"

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace language::parser;

namespace probfd::cli::heuristics {

InternalFunctionDefinitionBase&
add_blind_heuristic_factory_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskHeuristicFactory,
        BlindHeuristicFactory>>(nspace, "blind_heuristic");

    f.document_title("Blind Heuristic");
    f.document_synopsis(
        "This heuristic always returns an estimate of 0 for every state.");

    return f;
}

} // namespace probfd::cli::heuristics
