#include "downward/cli/evaluators/sum_evaluator_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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
        if (this->evaluator_factories.empty()) {
            throw std::domain_error("List of evaluators may not be empty.");
        }
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
    : public SharedTypedFeature<
          TaskDependentFactory<Evaluator>,
          std::string,
          Verbosity,
          vector<shared_ptr<TaskDependentFactory<Evaluator>>>> {
public:
    SumEvaluatorFeature()
        : TypedFeature("sum", &SumEvaluatorFeature::func)
    {
        document_title("Sum evaluator");
        document_synopsis("Calculates the sum of the sub-evaluators.");

        add_combining_evaluator_options_to_feature(*this, "sum", 0);
    }

    static shared_ptr<TaskDependentFactory<Evaluator>> func(
        std::string description,
        Verbosity verbosity,
        vector<shared_ptr<TaskDependentFactory<Evaluator>>> evaluator_factories)
    {
        return make_shared_from_arg_tuples<SumEvaluatorFactory>(
            std::move(description),
            verbosity,
            std::move(evaluator_factories));
    }
};

} // namespace

namespace downward::cli::evaluators {

void add_sum_evaluator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    const Feature& f = n.insert_feature_plugin<SumEvaluatorFeature>();
    SubcategoryPlugin& subcategory =
        registry.get_subcategory_plugin("evaluators_basic");
    subcategory.add_feature(f);
}

} // namespace downward::cli::evaluators
