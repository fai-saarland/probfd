#include "downward/cli/heuristics/max_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/heuristics/max_heuristic.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::max_heuristic;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;

namespace {
class HMaxHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;

public:
    HMaxHeuristicFactory(
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
        return std::make_unique<HSPMaxHeuristic>(
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class HSPMaxHeuristicFeature
    : public SharedTypedFeature<
          TaskDependentFactory<Evaluator>,
          shared_ptr<TaskTransformation>,
          bool,
          string,
          utils::Verbosity> {
public:
    HSPMaxHeuristicFeature()
        : TypedFeature("hmax", &HSPMaxHeuristicFeature::func)
    {
        document_title("Max heuristic");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "supported");
        document_language_support(
            "axioms",
            "supported (in the sense that the planner won't complain -- "
            "handling of axioms might be very stupid "
            "and even render the heuristic unsafe)");

        document_property("admissible", "yes for tasks without axioms");
        document_property("consistent", "yes for tasks without axioms");
        document_property("safe", "yes for tasks without axioms");
        document_property("preferred operators", "no");

        add_heuristic_options_to_feature(*this, "hmax", 0);
    }

    static shared_ptr<TaskDependentFactory<Evaluator>> func(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<HMaxHeuristicFactory>(
            std::move(transformation),
            cache_estimates,
            std::move(description),
            verbosity);
    }
};
} // namespace

namespace downward::cli::heuristics {

void add_max_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<HSPMaxHeuristicFeature>();
}

} // namespace downward::cli::heuristics
