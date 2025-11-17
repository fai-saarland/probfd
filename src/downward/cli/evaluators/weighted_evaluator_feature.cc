#include "downward/cli/evaluators/weighted_evaluator_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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
    : public SharedTypedFeature<
          TaskDependentFactory<Evaluator>,
          std::string,
          Verbosity,
          std::shared_ptr<TaskDependentFactory<Evaluator>>,
          int> {
public:
    WeightedEvaluatorFeature()
        : TypedFeature("weight", &WeightedEvaluatorFeature::func)
    {
        document_title("Weighted evaluator");
        document_synopsis(
            "Multiplies the value of the evaluator with the given weight.");

        make_required_argument(0, "eval", "evaluator");
        make_required_argument(1, "weight", "weight");
        add_evaluator_options_to_feature(*this, "weight", 2);
    }

    static shared_ptr<TaskDependentFactory<Evaluator>> func(
        const Context&,
        std::string description,
        Verbosity verbosity,
        std::shared_ptr<TaskDependentFactory<Evaluator>> eval_factory,
        int weight)
    {
        return make_shared_from_arg_tuples<WeightedEvaluatorFactory>(
            std::move(description),
            verbosity,
            std::move(eval_factory),
            weight);
    }
};

} // namespace

namespace downward::cli::evaluators {

void add_weighted_evaluator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    const Feature& f = n.insert_feature_plugin<WeightedEvaluatorFeature>();
    SubcategoryPlugin& subcategory =
        registry.get_subcategory_plugin("evaluators_basic");
    subcategory.add_feature(f);
}

} // namespace downward::cli::evaluators
