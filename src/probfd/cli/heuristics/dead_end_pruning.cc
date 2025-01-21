#include "downward/cli/plugins/plugin.h"

#include "probfd/heuristics/dead_end_pruning.h"

using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace downward::cli::plugins;

namespace {

class DeadEndPruningHeuristicFactoryFeature
    : public TypedFeature<
          TaskEvaluatorFactory,
          DeadEndPruningHeuristicFactory> {
public:
    DeadEndPruningHeuristicFactoryFeature()
        : TypedFeature("prune_dead_ends")
    {
        document_title("Dead-End Pruning Heuristic");
        document_synopsis(
            "This heuristic applies a classical planning heuristic h on the "
            "all-outcomes determinization of the planning task. "
            "The estimate for a state s returned by this heuristic is infinity "
            "if h(s) is infinity in the all-outcomes determinization. "
            "Otherwise, the heuristic value is 0.");

        add_option<std::shared_ptr<::Evaluator>>("evaluator");
    }

protected:
    std::shared_ptr<DeadEndPruningHeuristicFactory>
    create_component(const Options& options, const Context&) const override
    {
        return std::make_shared<DeadEndPruningHeuristicFactory>(
            options.get<std::shared_ptr<::Evaluator>>("evaluator"));
    }
};

FeaturePlugin<DeadEndPruningHeuristicFactoryFeature> _plugin;

} // namespace
