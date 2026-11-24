#include "probfd_cli/heuristics/ocp_heuristic.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward_cli/lp/lp_solver_options.h"

#include "probfd/heuristics/ocp_heuristic.h"

using namespace downward;

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace language;
using namespace language::plugins;

namespace {

class OptimalCostPartitioningHeuristicFactoryFeature
    : public SharedTypedFeature<TaskHeuristicFactory> {
public:
    OptimalCostPartitioningHeuristicFactoryFeature()
        : TypedFeature("ocp_heuristic")
    {
        document_title("Saturated Cost Partitioning Heuristic");
        document_synopsis(
            "This heuristic computes the optimal cost-partitioning heuristic "
            "over a set of projections.");

        downward::cli::lp::add_lp_solver_option_to_feature(*this);

        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "The pattern generation algorithm to construct the projections.",
            "classical_generator(generator=systematic(pattern_max_size=2))");
    }

    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<OCPHeuristicFactory>(
            downward::cli::lp::get_lp_solver_arguments_from_options(
                context,
                opts),
            opts.get<std::shared_ptr<PatternCollectionGenerator>>(
                context,
                "patterns"));
    }
};

} // namespace

namespace probfd::cli::heuristics {

void add_ocp_heuristic_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<
        OptimalCostPartitioningHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
