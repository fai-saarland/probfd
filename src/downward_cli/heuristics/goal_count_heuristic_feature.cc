#include "downward_cli/heuristics/goal_count_heuristic_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward_cli/heuristics/heuristic_options.h"

#include "downward/heuristics/goal_count_heuristic.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward::goal_count_heuristic;
using namespace downward::utils;

using namespace downward;

using namespace language;
using namespace language::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

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
    : public TypedFeature<TaskDependentFactory<Evaluator>> {
public:
    GoalCountHeuristicFeature()
        : TypedFeature("goalcount")
    {
        document_title("Goal count heuristic");

        add_heuristic_options_to_feature(*this, "goalcount");

        document_language_support("action costs", "ignored by design");
        document_language_support("conditional effects", "supported");
        document_language_support("axioms", "supported");

        document_property("admissible", "no");
        document_property("consistent", "no");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    shared_ptr<TaskDependentFactory<Evaluator>>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<GoalCountHeuristicFactory>(
            get_heuristic_arguments_from_options(context,opts));
    }
};
} // namespace

namespace downward::cli::heuristics {

void add_goal_count_heuristic_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<GoalCountHeuristicFeature>();
}

} // namespace downward::cli::heuristics
