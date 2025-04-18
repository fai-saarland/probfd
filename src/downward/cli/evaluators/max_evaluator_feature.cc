#include "downward/cli/plugins/plugin.h"

#include "downward/cli/evaluators/combining_evaluator_options.h"

#include "downward/evaluators/max_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;

using namespace downward;
using namespace downward::utils;
using namespace downward::max_evaluator;

using namespace downward::cli;
using namespace downward::cli::plugins;

using downward::cli::combining_evaluator ::
    add_combining_evaluator_options_to_feature;

using downward::cli::combining_evaluator ::
    get_combining_evaluator_arguments_from_options;

namespace {

class MaxEvaluatorFactory : public TaskDependentFactory<Evaluator> {
    std::string description;
    Verbosity verbosity;
    vector<shared_ptr<TaskDependentFactory<Evaluator>>> evaluator_factories;

public:
    MaxEvaluatorFactory(
        std::string description,
        Verbosity verbosity,
        vector<shared_ptr<TaskDependentFactory<Evaluator>>> evaluator_factories)
        : description(std::move(description))
        , verbosity(verbosity)
        , evaluator_factories(std::move(evaluator_factories))
    {
    }

    unique_ptr<Evaluator>
    create_object(const std::shared_ptr<AbstractTask>& task) override
    {
        std::vector<std::shared_ptr<Evaluator>> evaluators;

        for (const auto& factory : evaluator_factories) {
            evaluators.push_back(factory->create_object(task));
        }

        return std::make_unique<MaxEvaluator>(
            std::move(evaluators),
            description,
            verbosity);
    }
};

class MaxEvaluatorFeature
    : public TypedFeature<
          TaskDependentFactory<Evaluator>,
          MaxEvaluatorFactory> {
public:
    MaxEvaluatorFeature()
        : TypedFeature("max")
    {
        document_subcategory("evaluators_basic");
        document_title("Max evaluator");
        document_synopsis("Calculates the maximum of the sub-evaluators.");
        add_combining_evaluator_options_to_feature(*this, "max");
    }

    shared_ptr<MaxEvaluatorFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        verify_list_non_empty<shared_ptr<Evaluator>>(context, opts, "evals");
        return make_shared_from_arg_tuples<MaxEvaluatorFactory>(
            get_combining_evaluator_arguments_from_options(opts));
    }
};

FeaturePlugin<MaxEvaluatorFeature> _plugin;

} // namespace
