#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/heuristics/task_dependent_heuristic.h"

#include "probfd/heuristics/ucp_heuristic.h"

using namespace utils;

using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace downward::cli::plugins;

namespace {

class UCPHeuristicFactoryFeature
    : public TypedFeature<TaskEvaluatorFactory, UCPHeuristicFactory> {
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
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<UCPHeuristicFactory>(
            get_task_dependent_heuristic_arguments_from_options(opts),
            opts.get<std::shared_ptr<PatternCollectionGenerator>>("patterns"));
    }
};

FeaturePlugin<UCPHeuristicFactoryFeature> _plugin;

} // namespace
