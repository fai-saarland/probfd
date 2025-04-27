#include "downward/cli/plugins/plugin.h"
#include "downward/task_transformation.h"

#include "downward/cli/heuristic_options.h"

#include "downward/heuristics/additive_heuristic.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward::utils;
using namespace downward::additive_heuristic;

using namespace downward;
using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

namespace {

class AdditiveHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;

public:
    AdditiveHeuristicFactory(
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
        return std::make_unique<AdditiveHeuristic>(
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

class AdditiveHeuristicFeature
    : public TypedFeature<
          TaskDependentFactory<Evaluator>,
          AdditiveHeuristicFactory> {
public:
    AdditiveHeuristicFeature()
        : TypedFeature("add")
    {
        document_title("Additive heuristic");

        add_heuristic_options_to_feature(*this, "add");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "supported");
        document_language_support(
            "axioms",
            "supported (in the sense that the planner won't complain -- "
            "handling of axioms might be very stupid "
            "and even render the heuristic unsafe)");

        document_property("admissible", "no");
        document_property("consistent", "no");
        document_property("safe", "yes for tasks without axioms");
        document_property("preferred operators", "yes");
    }

    shared_ptr<AdditiveHeuristicFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<AdditiveHeuristicFactory>(
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<AdditiveHeuristicFeature> _plugin;

} // namespace
