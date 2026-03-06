#include "downward/cli/evaluators/g_evaluator_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/cli/evaluators/evaluator_options.h"

#include "downward/evaluators/g_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::g_evaluator;

using namespace downward::cli;
using namespace language::parser;

namespace {

class GEvaluatorFactory : public TaskDependentFactory<Evaluator> {
    std::string description;
    Verbosity verbosity;

public:
    GEvaluatorFactory(std::string description, Verbosity verbosity)
        : description(std::move(description))
        , verbosity(verbosity)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask&) override
    {
        return std::make_unique<GEvaluator>(description, verbosity);
    }
};

} // namespace

namespace downward::cli::evaluators {

InternalFunctionDefinitionBase&
add_g_evaluator_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskDependentFactory<Evaluator>,
        GEvaluatorFactory,
        std::string,
        Verbosity>>(nspace, "g");

    f.document_title("g-value evaluator");
    f.document_synopsis("Returns the g-value (path cost) of the search node.");

    add_evaluator_options_to_feature(f, "g", 0);

    return f;
}

} // namespace downward::cli::evaluators
