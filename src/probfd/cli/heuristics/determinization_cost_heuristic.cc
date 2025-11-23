#include "probfd/cli/heuristics/determinization_cost_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/heuristics/determinization_cost_heuristic.h"

#include <downward/task_dependent_factory.h>

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace downward::cli::plugins;

namespace {

Feature& add_determinization_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "det",
        &downward::cli::plugins::make_shared<
            TaskHeuristicFactory,
            DeterminizationCostHeuristicFactory,
            std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>);

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

} // namespace

namespace probfd::cli::heuristics {

void add_determinization_cost_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_determinization_heuristic_to_namespace(n);
}

} // namespace probfd::cli::heuristics
