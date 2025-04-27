#include "downward/cli/plugins/plugin.h"

#include "downward/cli/heuristic_options.h"

#include "downward/heuristics/cea_heuristic.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::cea_heuristic;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

namespace {

class ContextEnhancedAdditiveHeuristicFactory
    : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;

public:
    ContextEnhancedAdditiveHeuristicFactory(
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

    unique_ptr<Evaluator>
    create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);
        return std::make_unique<ContextEnhancedAdditiveHeuristic>(
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class ContextEnhancedAdditiveHeuristicFeature
    : public TypedFeature<
          TaskDependentFactory<Evaluator>,
          ContextEnhancedAdditiveHeuristicFactory> {
public:
    ContextEnhancedAdditiveHeuristicFeature()
        : TypedFeature("cea")
    {
        document_title("Context-enhanced additive heuristic");

        add_heuristic_options_to_feature(*this, "cea");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "supported");
        document_language_support(
            "axioms",
            "supported (in the sense that the planner won't complain -- "
            "handling of axioms might be very stupid "
            "and even render the heuristic unsafe)");

        document_property("admissible", "no");
        document_property("consistent", "no");
        document_property("safe", "no");
        document_property("preferred operators", "yes");
    }

    shared_ptr<ContextEnhancedAdditiveHeuristicFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<
            ContextEnhancedAdditiveHeuristicFactory>(
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<ContextEnhancedAdditiveHeuristicFeature> _plugin;

} // namespace
