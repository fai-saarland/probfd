#include "probfd/cli/heuristics/dead_end_pruning_heuristic.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/heuristics/dead_end_pruning_heuristic.h"

using namespace downward;

using namespace probfd;
using namespace probfd::heuristics;

using namespace language::plugins;

namespace probfd::cli::heuristics {

InternalFunctionDefinitionBase&
add_dead_end_pruning_heuristic_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "prune_dead_ends",
        &construct_shared<
            TaskHeuristicFactory,
            DeadEndPruningHeuristicFactory,
            std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>);

    f.document_title("Dead-End Pruning Heuristic");
    f.document_synopsis(
        "This heuristic applies a classical planning heuristic h on the "
        "all-outcomes determinization of the planning task. "
        "The estimate for a state s returned by this heuristic is infinity "
        "if h(s) is infinity in the all-outcomes determinization. "
        "Otherwise, the heuristic value is 0.");

    f.make_required_argument(0, "evaluator");

    return f;
}

} // namespace probfd::cli::heuristics
