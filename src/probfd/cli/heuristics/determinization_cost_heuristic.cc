#include "probfd/cli/heuristics/determinization_cost_heuristic.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/heuristics/determinization_cost_heuristic.h"

#include "downward/task_dependent_factory.h"

using namespace downward;

using namespace probfd;
using namespace probfd::heuristics;

using namespace language::parser;

namespace probfd::cli::heuristics {

InternalFunctionDefinitionBase& add_determinization_cost_heuristic_feature(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&construct_shared<
        TaskHeuristicFactory,
        DeterminizationCostHeuristicFactory,
        std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>>(
        nspace,
        "det");

    f.document_title("Determinization-based Heuristic");
    f.document_synopsis(
        "This heuristic returns the estimate of a classical "
        "planning heuristic evaluated on the all-outcomes "
        "determinization of the planning task.");

    f.make_required_argument(
        0,
        "heuristic",
        "The classical planning heuristic.");

    return f;
}

} // namespace probfd::cli::heuristics
