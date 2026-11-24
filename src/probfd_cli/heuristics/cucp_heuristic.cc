#include "probfd_cli/heuristics/cucp_heuristic.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd_cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/cucp_heuristic.h"

using namespace downward;

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace language;
using namespace language::plugins;

namespace {
class CUCPHeuristicFactoryFeature
    : public SharedTypedFeature<TaskHeuristicFactory> {
public:
    CUCPHeuristicFactoryFeature()
        : TypedFeature("cucp_heuristic")
    {
        document_title("Canonical Uniform Cost Partitioning Heuristic");
        document_synopsis(
            "This heuristic computes a uniform cost-partitioning estimate "
            "over a set of projections that only considers pattern affected by "
            "a given operator. "
            "Given n projections, every corresponding projection heuristic is "
            "constructed under the cost function that assigns each operator o "
            "the cost 1/m * c(o) if o affects the projection and 0 otherwise, "
            "where c(o) is the original operator cost of o according to the "
            "planning task and m is the number of total affected patterns. "
            "The estimate of a state is the sum over all estimates of these "
            "projection heuristics for this state.");

        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "The pattern generation algorithm to construct the projections.",
            "classical_generator(generator=systematic(pattern_max_size=2))");

        add_task_dependent_heuristic_options_to_feature(*this);
    }

    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<CUCPHeuristicFactory>(
            get_task_dependent_heuristic_arguments_from_options(context, opts),
            opts.get<std::shared_ptr<PatternCollectionGenerator>>(
                context,
                "patterns"));
    }
};
} // namespace

namespace probfd::cli::heuristics {

void add_cucp_heuristic_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<CUCPHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
