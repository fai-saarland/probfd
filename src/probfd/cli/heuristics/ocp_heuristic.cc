#include "probfd/cli/heuristics/ocp_heuristic.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"
#include "downward/cli/lp/lp_solver_options.h"

#include "probfd/heuristics/ocp_heuristic.h"

using namespace downward;

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace language::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

InternalFunctionDefinitionBase&
add_optimal_cost_partitioning_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "ocp_heuristic",
        &language::plugins::construct_shared<
            TaskHeuristicFactory,
            OCPHeuristicFactory,
            lp::LPSolverType,
            std::shared_ptr<PatternCollectionGenerator>>);

    f.document_title("Saturated Cost Partitioning Heuristic");
    f.document_synopsis(
        "This heuristic computes a saturated cost-partitioning estimate "
        "over a set of projections. "
        "The construction algorithm maintains a running cost function that "
        "is initially the original cost function of the planning task. "
        "The projections are ordered and their projection heuristic under "
        "the running cost function are constructed in sequence. "
        "Once a projection heuristic is computed, its saturated costs are "
        "computed, i.e., the (unique) minimal operator costs such that the "
        "projection heuristic would not change. "
        "The saturated costs are then substracted from the running cost "
        "function. "
        "This process continues after all projection heuristics have been "
        "constructed. "
        "The estimate of a state is the sum over all projection heuristic "
        "estimates for this state.");

    const auto n = downward::cli::lp::add_lp_solver_option_to_feature(f, 0);

    f.make_optional_argument_with_default(
        n,
        "patterns",
        "classical_generator(generator=systematic(pattern_max_size=2))",
        "The pattern generation algorithm to construct the projections.");

    return f;
}

} // namespace

namespace probfd::cli::heuristics {

void add_ocp_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();

    add_optimal_cost_partitioning_heuristic_to_namespace(n);
}

} // namespace probfd::cli::heuristics
