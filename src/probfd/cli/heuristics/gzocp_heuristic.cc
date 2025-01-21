#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/gzocp_heuristic.h"

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {

class GZOCPHeuristicFactoryFeature
    : public TypedFeature<TaskEvaluatorFactory, GZOCPHeuristicFactory> {
public:
    GZOCPHeuristicFactoryFeature()
        : TypedFeature("gzocp_heuristic")
    {
        document_title("Greedy Zero-One Operator Cost Partitioning Heuristic");
        document_synopsis(
            "This heuristic computes a greedy cost-partitioning estimate over "
            "a set of projections. The cost-partitioning assigns the full "
            "operator cost to the first projection in the sequence that is "
            "affected by it (induces a non-self-loop) and assigns a cost of "
            "zero for this operator for all subsequent projections.");

        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "The pattern generation algorithm to construct the projections.",
            "classical_generator(generator=systematic(pattern_max_size=2))");
        add_option<GZOCPHeuristic::OrderingStrategy>(
            "order",
            "The order in which the projections are considered.",
            "random");

        add_rng_options_to_feature(*this);
        add_task_dependent_heuristic_options_to_feature(*this);
    }

    std::shared_ptr<GZOCPHeuristicFactory>
    create_component(const Options& opts, const utils::Context&) const override
    {
        return make_shared_from_arg_tuples<GZOCPHeuristicFactory>(
            opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"),
            opts.get<GZOCPHeuristic::OrderingStrategy>("order"),
            get_rng_arguments_from_options(opts),
            get_task_dependent_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<GZOCPHeuristicFactoryFeature> _plugin;

TypedEnumPlugin<GZOCPHeuristic::OrderingStrategy> _enum_plugin(
    {{"random", "the order is random"},
     {"size_asc", "orders the PDBs by increasing size"},
     {"size_desc", "orders the PDBs by decreasing size"},
     {"inherit",
      "inherits the order from the underlying pattern generation algorithm"}});

} // namespace
