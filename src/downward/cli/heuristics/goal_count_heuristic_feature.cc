#include "downward/cli/heuristics/goal_count_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/heuristics/goal_count_heuristic.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward::goal_count_heuristic;
using namespace downward::utils;

using namespace downward;
using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;

namespace {
class GoalCountHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;

public:
    GoalCountHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);
        return std::make_unique<GoalCountHeuristic>(
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class GoalCountHeuristicFeature
    : public SharedTypedFeature<
          TaskDependentFactory<Evaluator>,
          shared_ptr<TaskTransformation>,
          bool,
          string,
          utils::Verbosity> {
public:
    GoalCountHeuristicFeature()
        : TypedFeature("goalcount", &GoalCountHeuristicFeature::func)
    {
        document_title("Goal count heuristic");

        document_language_support("action costs", "ignored by design");
        document_language_support("conditional effects", "supported");
        document_language_support("axioms", "supported");

        document_property("admissible", "no");
        document_property("consistent", "no");
        document_property("safe", "yes");
        document_property("preferred operators", "no");

        add_heuristic_options_to_feature(*this, "goalcount", 0);
    }

    static shared_ptr<TaskDependentFactory<Evaluator>> func(
        const Context&,
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity)
    {
        return make_shared<GoalCountHeuristicFactory>(
            std::move(transformation),
            cache_estimates,
            std::move(description),
            verbosity);
    }
};
} // namespace

namespace downward::cli::heuristics {

void add_goal_count_heuristic_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<GoalCountHeuristicFeature>();
}

} // namespace downward::cli::heuristics
