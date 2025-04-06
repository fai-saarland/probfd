#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/ucp_heuristic.h"

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace downward::cli::plugins;

namespace {

class UCPHeuristicFactoryFeature
    : public TypedFeature<TaskHeuristicFactory, UCPHeuristicFactory> {
public:
    UCPHeuristicFactoryFeature()
        : TypedFeature("ucp_heuristic")
    {
        document_title("Uniform Cost Partitioning Heuristic");
        document_synopsis(
            "This heuristic computes a uniform cost-partitioning estimate "
            "over a set of projections. "
            "Given n projections, every corresponding projection heuristic is "
            "constructed under the cost function that assigns each operator o "
            "the cost 1/n * c(o), where c(o) is the original operator cost of "
            "o according to the planning task. "
            "The estimate of a state is the sum over all estimates of these "
            "projection heuristics for this state.");

        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "The pattern generation algorithm to construct the projections.",
            "classical_generator(generator=systematic(pattern_max_size=2))");

        add_task_dependent_heuristic_options_to_feature(*this);
    }

    std::shared_ptr<UCPHeuristicFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<UCPHeuristicFactory>(
            get_task_dependent_heuristic_arguments_from_options(opts),
            opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"));
    }
};

FeaturePlugin<UCPHeuristicFactoryFeature> _plugin;

} // namespace
