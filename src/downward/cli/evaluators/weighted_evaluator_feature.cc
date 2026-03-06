#include "downward/cli/evaluators/weighted_evaluator_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/cli/evaluators/evaluator_options.h"

#include "downward/evaluators/weighted_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::weighted_evaluator;

using namespace language::parser;

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

} // namespace

namespace downward::cli::evaluators {

InternalFunctionDefinitionBase&
add_weighted_evaluator_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskDependentFactory<Evaluator>,
        WeightedEvaluatorFactory,
        std::string,
        Verbosity,
        shared_ptr<TaskDependentFactory<Evaluator>>,
        int>>(nspace, "weight");

    f.document_title("Weighted evaluator");
    f.document_synopsis(
        "Multiplies the value of the evaluator with the given weight.");

    f.make_required_argument(0, "eval", "evaluator");
    f.make_required_argument(1, "weight", "weight");
    add_evaluator_options_to_feature(f, "weight", 2);

    return f;
}

} // namespace downward::cli::evaluators
