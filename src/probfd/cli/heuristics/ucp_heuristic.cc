#include "probfd/cli/heuristics/ucp_heuristic.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/ucp_heuristic.h"

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace language::plugins;

namespace {

InternalFunctionDefinitionBase& add_uniform_cost_partitioning_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "ucp_heuristic",
        &language::plugins::construct_shared<
            TaskHeuristicFactory,
            UCPHeuristicFactory,
            std::shared_ptr<PatternCollectionGenerator>>);

    f.document_title("Uniform Cost Partitioning Heuristic");
    f.document_synopsis(
        "This heuristic computes a uniform cost-partitioning estimate "
        "over a set of projections. "
        "Given n projections, every corresponding projection heuristic is "
        "constructed under the cost function that assigns each operator o "
        "the cost 1/n * c(o), where c(o) is the original operator cost of "
        "o according to the planning task. "
        "The estimate of a state is the sum over all estimates of these "
        "projection heuristics for this state.");

    f.make_optional_argument_with_default(
        0,
        "patterns",
        "classical_generator(generator=systematic(pattern_max_size=2))",
        "The pattern generation algorithm to construct the projections.");

    return f;
}

} // namespace

namespace probfd::cli::heuristics {

void add_ucp_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_uniform_cost_partitioning_heuristic_to_namespace(n);
}

} // namespace probfd::cli::heuristics
