#include "downward/cli/evaluators/weighted_evaluator_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/cli/evaluators/evaluator_options.h"

#include "downward/evaluators/weighted_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::weighted_evaluator;

using namespace downward::cli::plugins;

using downward::cli::add_evaluator_options_to_feature;
using downward::cli::get_evaluator_arguments_from_options;

namespace {

class WeightedEvaluatorFactory : public TaskDependentFactory<Evaluator> {
    std::string description;
    Verbosity verbosity;
    std::shared_ptr<TaskDependentFactory<Evaluator>> eval_factory;
    int weight;

public:
    WeightedEvaluatorFactory(
        std::string description,
        Verbosity verbosity,
        std::shared_ptr<TaskDependentFactory<Evaluator>> eval_factory,
        int weight)
        : description(std::move(description))
        , verbosity(verbosity)
        , eval_factory(std::move(eval_factory))
        , weight(weight)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto eval = eval_factory->create_object(task);
        return std::make_unique<WeightedEvaluator>(
            std::move(eval),
            weight,
            description,
            verbosity);
    }
};

class WeightedEvaluatorFeature
    : public TypedFeature<TaskDependentFactory<Evaluator>> {
public:
    WeightedEvaluatorFeature()
        : TypedFeature("weight")
    {
        document_subcategory("evaluators_basic");
        document_title("Weighted evaluator");
        document_synopsis(
            "Multiplies the value of the evaluator with the given weight.");

        add_option<shared_ptr<TaskDependentFactory<Evaluator>>>(
            "eval",
            "evaluator");
        add_option<int>("weight", "weight");
        add_evaluator_options_to_feature(*this, "weight");
    }

    shared_ptr<TaskDependentFactory<Evaluator>>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<WeightedEvaluatorFactory>(
            get_evaluator_arguments_from_options(opts),
            opts.get<shared_ptr<TaskDependentFactory<Evaluator>>>("eval"),
            opts.get<int>("weight"));
    }
};

} // namespace

namespace downward::cli::evaluators {

void add_weighted_evaluator_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<WeightedEvaluatorFeature>();
}

} // namespace downward::cli::evaluators
