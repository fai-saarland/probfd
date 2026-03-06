#include "downward/cli/evaluators/sum_evaluator_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/cli/evaluators/combining_evaluator_options.h"

#include "downward/evaluators/sum_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::sum_evaluator;

using namespace language::parser;

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

} // namespace

namespace downward::cli::evaluators {

InternalFunctionDefinitionBase&
add_sum_evaluator_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskDependentFactory<Evaluator>,
        SumEvaluatorFactory,
        std::string,
        Verbosity,
        vector<shared_ptr<TaskDependentFactory<Evaluator>>>>>(nspace, "sum");

    f.document_title("Sum evaluator");
    f.document_synopsis("Calculates the sum of the sub-evaluators.");

    add_combining_evaluator_options_to_feature(f, "sum", 0);

    return f;
}

} // namespace downward::cli::evaluators
