#include "probfd/cli/heuristics/dead_end_pruning_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/heuristics/dead_end_pruning_heuristic.h"

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace downward::cli::plugins;

namespace {
class DeadEndPruningHeuristicFactoryFeature
    : public SharedTypedFeature<
          TaskHeuristicFactory,
          std::shared_ptr<downward::TaskDependentFactory<Evaluator>>> {
public:
    DeadEndPruningHeuristicFactoryFeature()
        : TypedFeature(
              "prune_dead_ends",
              &DeadEndPruningHeuristicFactoryFeature::func)
    {
        document_title("Dead-End Pruning Heuristic");
        document_synopsis(
            "This heuristic applies a classical planning heuristic h on the "
            "all-outcomes determinization of the planning task. "
            "The estimate for a state s returned by this heuristic is infinity "
            "if h(s) is infinity in the all-outcomes determinization. "
            "Otherwise, the heuristic value is 0.");

        make_required_argument(0, "evaluator");
    }

protected:
    static std::shared_ptr<TaskHeuristicFactory> func(
        std::shared_ptr<downward::TaskDependentFactory<Evaluator>> evaluator)
    {
        return std::make_shared<DeadEndPruningHeuristicFactory>(
            std::move(evaluator));
    }
};
} // namespace

namespace probfd::cli::heuristics {

void add_dead_end_pruning_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<DeadEndPruningHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
