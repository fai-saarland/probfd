#include "probfd_plugins/heuristics/task_dependent_heuristic.h"

#include "probfd/heuristics/gzocp_heuristic.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd_plugins::heuristics;

namespace {

class GZOCPHeuristicFactoryFeature
    : public plugins::
          TypedFeature<TaskEvaluatorFactory, GZOCPHeuristicFactory> {
public:
    GZOCPHeuristicFactoryFeature()
        : TypedFeature("gzocp_heuristic")
    {
        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "The pattern generation algorithm.",
            "classical_generator(generator=systematic(pattern_max_size=2))");
        add_option<GZOCPHeuristic::OrderingStrategy>(
            "order",
            "The order in which patterns are considered",
            "random");

        utils::add_rng_options_to_feature(*this);
        add_task_dependent_heuristic_options_to_feature(*this);
    }

    std::shared_ptr<GZOCPHeuristicFactory>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<GZOCPHeuristicFactory>(
            opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"),
            opts.get<GZOCPHeuristic::OrderingStrategy>("order"),
            utils::get_rng_arguments_from_options(opts),
            get_task_dependent_heuristic_arguments_from_options(opts));
    }
};

plugins::FeaturePlugin<GZOCPHeuristicFactoryFeature> _plugin;

plugins::TypedEnumPlugin<GZOCPHeuristic::OrderingStrategy> _enum_plugin(
    {{"random", "the order is random"},
     {"size_asc", "orders the PDBs by increasing size"},
     {"size_desc", "orders the PDBs by decreasing size"},
     {"inherit",
      "inherits the order from the underlying pattern generation algorithm"}});

} // namespace
