#include "downward/cli/evaluators/max_evaluator_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/evaluators/combining_evaluator_options.h"

#include "downward/evaluators/max_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;

using namespace downward;
using namespace downward::utils;
using namespace downward::max_evaluator;

using namespace downward::cli;
using namespace language::plugins;

using downward::cli::combining_evaluator ::
    add_combining_evaluator_options_to_feature;

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

        return std::make_unique<MaxEvaluator>(
            std::move(evaluators),
            description,
            verbosity);
    }
};

} // namespace

namespace downward::cli::evaluators {

InternalFunctionDefinitionBase& add_max_evaluator_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "max",
        &language::plugins::construct_shared<
            TaskDependentFactory<Evaluator>,
            MaxEvaluatorFactory,
            std::string,
            Verbosity,
            vector<shared_ptr<TaskDependentFactory<Evaluator>>>>);
    f.document_title("Max evaluator");
    f.document_synopsis("Calculates the maximum of the sub-evaluators.");
    add_combining_evaluator_options_to_feature(f, "max", 0);

    return f;
}

} // namespace downward::cli::evaluators
