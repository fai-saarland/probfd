#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/utils/rng_options.h"

#include "probfd_plugins/heuristics/task_dependent_heuristic.h"

#include "probfd/heuristics/scp_heuristic.h"

using namespace utils;

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd_plugins::heuristics;

using namespace downward_plugins::plugins;

using downward_plugins::utils::add_rng_options_to_feature;
using downward_plugins::utils::get_rng_arguments_from_options;

namespace {

class SCPHeuristicFactoryFeature
    : public TypedFeature<TaskEvaluatorFactory, SCPHeuristicFactory> {
public:
    SCPHeuristicFactoryFeature()
        : TypedFeature("scp_heuristic")
    {
        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "The pattern generation algorithm.",
            "classical_generator(generator=systematic(pattern_max_size=2))");
        add_option<SCPHeuristic::OrderingStrategy>(
            "order",
            "The order in which patterns are considered",
            "random");

        add_rng_options_to_feature(*this);
        add_task_dependent_heuristic_options_to_feature(*this);
    }

    std::shared_ptr<SCPHeuristicFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<SCPHeuristicFactory>(
            opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"),
            opts.get<SCPHeuristic::OrderingStrategy>("order"),
            get_rng_arguments_from_options(opts),
            get_task_dependent_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<SCPHeuristicFactoryFeature> _plugin;

TypedEnumPlugin<SCPHeuristic::OrderingStrategy> _enum_plugin(
    {{"random", "the order is random"},
     {"size_asc", "orders the PDBs by increasing size"},
     {"size_desc", "orders the PDBs by decreasing size"},
     {"inherit",
      "inherits the order from the underlying pattern generation algorithm"}});

} // namespace
