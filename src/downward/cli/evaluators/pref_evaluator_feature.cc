#include "downward/cli/evaluators/pref_evaluator_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/cli/evaluators/evaluator_options.h"

#include "downward/evaluators/pref_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::pref_evaluator;
using namespace downward::utils;

using namespace language::parser;

using downward::cli::add_evaluator_options_to_feature;

namespace {

class PrefEvaluatorFactory : public TaskDependentFactory<Evaluator> {
    std::string description;
    Verbosity verbosity;

public:
    PrefEvaluatorFactory(std::string description, Verbosity verbosity)
        : description(std::move(description))
        , verbosity(verbosity)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask&) override
    {
        return std::make_unique<PrefEvaluator>(description, verbosity);
    }
};

} // namespace

namespace downward::cli::evaluators {

InternalFunctionDefinitionBase&
add_pref_evaluator_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskDependentFactory<Evaluator>,
        PrefEvaluatorFactory,
        std::string,
        Verbosity>>(nspace, "pref");

    f.document_title("Preference evaluator");
    f.document_synopsis("Returns 0 if preferred is true and 1 otherwise.");
    add_evaluator_options_to_feature(f, "pref", 0);

    return f;
}

} // namespace downward::cli::evaluators
