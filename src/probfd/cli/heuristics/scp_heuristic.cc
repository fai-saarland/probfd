#include "probfd/cli/heuristics/scp_heuristic.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/scp_heuristic.h"

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace language::plugins;

using downward::cli::utils::add_rng_options_to_feature;

namespace {

InternalFunctionDefinitionBase&
add_saturated_cost_partitioning_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "scp_heuristic",
        &language::plugins::construct_shared<
            TaskHeuristicFactory,
            SCPHeuristicFactory,
            std::shared_ptr<PatternCollectionGenerator>,
            SCPHeuristicFactory::OrderingStrategy,
            std::shared_ptr<RandomNumberGenerator>>);

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

    f.make_optional_argument_with_default(
        0,
        "patterns",
        "classical_generator(generator=systematic(pattern_max_size=2))",
        "The pattern generation algorithm to construct the projections.");

    f.make_optional_argument_with_default(
        1,
        "order",
        "random",
        "The order in which the projections are considered.");

    add_rng_options_to_feature(f, 2);

    return f;
}

} // namespace

namespace probfd::cli::heuristics {

void add_scp_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_declaration<SCPHeuristicFactory::OrderingStrategy>(
        {{"random", "the order is random"},
         {"size_asc", "orders the PDBs by increasing size"},
         {"size_desc", "orders the PDBs by decreasing size"},
         {"inherit",
          "inherits the order from the underlying pattern generation "
          "algorithm"}});

    add_saturated_cost_partitioning_heuristic_to_namespace(n);
}

} // namespace probfd::cli::heuristics
