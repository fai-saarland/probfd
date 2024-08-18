#include "probfd_plugins/heuristics/task_dependent_heuristic.h"

#include "probfd/heuristics/ucp_heuristic.h"

#include "downward/plugins/plugin.h"

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd_plugins::heuristics;

namespace {

class UCPHeuristicFactoryFeature
    : public plugins::TypedFeature<TaskEvaluatorFactory, UCPHeuristicFactory> {
public:
    UCPHeuristicFactoryFeature()
        : TypedFeature("ucp_heuristic")
    {
        add_option<std::shared_ptr<PatternCollectionGenerator>>(
            "patterns",
            "The pattern generation algorithm.",
            "classical_generator(generator=systematic(pattern_max_size=2))");
        add_task_dependent_heuristic_options_to_feature(*this);
    }

    std::shared_ptr<UCPHeuristicFactory>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<UCPHeuristicFactory>(
            get_task_dependent_heuristic_arguments_from_options(opts),
            opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"));
    }
};

plugins::FeaturePlugin<UCPHeuristicFactoryFeature> _plugin;

} // namespace
