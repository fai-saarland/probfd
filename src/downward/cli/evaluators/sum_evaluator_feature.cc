#include "downward/cli/evaluators/sum_evaluator_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/evaluators/combining_evaluator_options.h"

#include "downward/evaluators/sum_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::sum_evaluator;

using namespace downward::cli::plugins;

using downward::cli::combining_evaluator::
    add_combining_evaluator_options_to_feature;

using downward::cli::combining_evaluator::
    get_combining_evaluator_arguments_from_options;

namespace {

class SumEvaluatorFactory : public TaskDependentFactory<Evaluator> {
    std::string description;
    Verbosity verbosity;
    vector<shared_ptr<TaskDependentFactory<Evaluator>>> evaluator_factories;

public:
    SumEvaluatorFactory(
        std::string description,
        Verbosity verbosity,
        vector<shared_ptr<TaskDependentFactory<Evaluator>>> evaluator_factories)
        : description(std::move(description))
        , verbosity(verbosity)
        , evaluator_factories(std::move(evaluator_factories))
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        std::vector<std::shared_ptr<Evaluator>> evaluators;

        for (const auto& factory : evaluator_factories) {
            evaluators.push_back(factory->create_object(task));
        }

        return std::make_unique<SumEvaluator>(
            std::move(evaluators),
            description,
            verbosity);
    }
};

class SumEvaluatorFeature
    : public SharedTypedFeature<TaskDependentFactory<Evaluator>> {
public:
    SumEvaluatorFeature()
        : SharedTypedFeature("sum")
    {
        document_subcategory("evaluators_basic");
        document_title("Sum evaluator");
        document_synopsis("Calculates the sum of the sub-evaluators.");

        add_combining_evaluator_options_to_feature(*this, "sum");
    }

    shared_ptr<TaskDependentFactory<Evaluator>>
    create_component(const Options& opts, const Context& context) const override
    {
        verify_list_non_empty<shared_ptr<TaskDependentFactory<Evaluator>>>(
            context,
            opts,
            "evals");
        return make_shared_from_arg_tuples<SumEvaluatorFactory>(
            get_combining_evaluator_arguments_from_options(opts));
    }
};

} // namespace

namespace downward::cli::evaluators {

void add_sum_evaluator_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<SumEvaluatorFeature>();
}

} // namespace downward::cli::evaluators
