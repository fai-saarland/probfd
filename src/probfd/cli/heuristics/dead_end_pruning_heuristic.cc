#include "probfd/cli/heuristics/dead_end_pruning_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/heuristics/dead_end_pruning_heuristic.h"

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace downward::cli::plugins;

namespace {
class DeadEndPruningHeuristicFactoryFeature
    : public SharedTypedFeature<TaskHeuristicFactory> {
public:
    DeadEndPruningHeuristicFactoryFeature()
        : SharedTypedFeature("prune_dead_ends")
    {
        document_title("Dead-End Pruning Heuristic");
        document_synopsis(
            "This heuristic applies a classical planning heuristic h on the "
            "all-outcomes determinization of the planning task. "
            "The estimate for a state s returned by this heuristic is infinity "
            "if h(s) is infinity in the all-outcomes determinization. "
            "Otherwise, the heuristic value is 0.");

        add_option<std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>(
            "evaluator");
    }

protected:
    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options& options, const Context&) const override
    {
        return std::make_shared<DeadEndPruningHeuristicFactory>(
            options.get<
                std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>(
                "evaluator"));
    }
};
} // namespace

namespace probfd::cli::heuristics {

void add_dead_end_pruning_heuristic_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<DeadEndPruningHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
