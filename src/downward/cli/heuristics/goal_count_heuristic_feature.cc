#include "downward/cli/heuristics/goal_count_heuristic_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

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

InternalFunctionDefinitionBase& add_goal_count_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "goalcount",
        &downward::cli::plugins::construct_shared<
            TaskDependentFactory<Evaluator>,
            GoalCountHeuristicFactory,
            shared_ptr<TaskTransformation>,
            bool,
            string,
            Verbosity>);

    f.document_title("Goal count heuristic");

    f.document_language_support("action costs", "ignored by design");
    f.document_language_support("conditional effects", "supported");
    f.document_language_support("axioms", "supported");

    f.document_property("admissible", "no");
    f.document_property("consistent", "no");
    f.document_property("safe", "yes");
    f.document_property("preferred operators", "no");

    add_heuristic_options_to_feature(f, "goalcount", 0);

    return f;
}

} // namespace

namespace downward::cli::heuristics {

void add_goal_count_heuristic_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_goal_count_heuristic_to_namespace(n);
}

} // namespace downward::cli::heuristics
