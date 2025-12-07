#include "downward/cli/evaluators/sum_evaluator_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

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

InternalFunctionDefinitionBase& add_sum_evaluator_feature_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "sum",
        &cli::plugins::construct_shared<
            TaskDependentFactory<Evaluator>,
            SumEvaluatorFactory,
            std::string,
            Verbosity,
            vector<shared_ptr<TaskDependentFactory<Evaluator>>>>);
    f.document_title("Sum evaluator");
    f.document_synopsis("Calculates the sum of the sub-evaluators.");

    add_combining_evaluator_options_to_feature(f, "sum", 0);

    return f;
}

} // namespace

namespace downward::cli::evaluators {

void add_sum_evaluator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    const InternalFunctionDefinitionBase& f = add_sum_evaluator_feature_to_namespace(n);
    DocumentationTopic& subcategory =
        registry.get_topic_by_name("evaluators_basic");
    subcategory.add_function(f);
}

} // namespace downward::cli::evaluators
