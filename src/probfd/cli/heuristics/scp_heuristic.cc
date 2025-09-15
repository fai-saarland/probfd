#include "probfd/cli/heuristics/scp_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/scp_heuristic.h"

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {
class SCPHeuristicFactoryFeature
    : public TypedFeature<TaskHeuristicFactory, SCPHeuristicFactory> {
public:
    SCPHeuristicFactoryFeature()
        : TypedFeature("scp_heuristic")
    {
        document_title("Saturated Cost Partitioning Heuristic");
        document_synopsis(
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

        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "The pattern generation algorithm to construct the projections.",
            "classical_generator(generator=systematic(pattern_max_size=2))");
        add_option<SCPHeuristicFactory::OrderingStrategy>(
            "order",
            "The order in which the projections are considered.",
            "random");

        add_rng_options_to_feature(*this);
        add_task_dependent_heuristic_options_to_feature(*this);
    }

    std::shared_ptr<SCPHeuristicFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<SCPHeuristicFactory>(
            opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"),
            opts.get<SCPHeuristicFactory::OrderingStrategy>("order"),
            get_rng_arguments_from_options(opts),
            get_task_dependent_heuristic_arguments_from_options(opts));
    }
};
}

namespace probfd::cli::heuristics {

void add_scp_heuristic_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<SCPHeuristicFactoryFeature>();

    raw_registry.insert_enum_plugin<SCPHeuristicFactory::OrderingStrategy>(
        {{"random", "the order is random"},
         {"size_asc", "orders the PDBs by increasing size"},
         {"size_desc", "orders the PDBs by decreasing size"},
         {"inherit",
          "inherits the order from the underlying pattern generation "
          "algorithm"}});
}

} // namespace
