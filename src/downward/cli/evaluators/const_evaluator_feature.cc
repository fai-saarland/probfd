#include "downward/cli/evaluators/const_evaluator_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/evaluators/evaluator_options.h"

#include "downward/evaluators/const_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::const_evaluator;

using namespace language::plugins;

using downward::cli::add_evaluator_options_to_feature;

namespace {
class ConstEvaluatorFactory : public TaskDependentFactory<Evaluator> {
    std::string description;
    Verbosity verbosity;
    int value;

public:
    ConstEvaluatorFactory(
        std::string description,
        Verbosity verbosity,
        int value)
        : description(std::move(description))
        , verbosity(verbosity)
        , value(value)
    {
        if (value < 0) {
            throw std::domain_error("constant value must be >= 0.");
        }
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask&) override
    {
        return std::make_unique<ConstEvaluator>(value, description, verbosity);
    }
};

InternalFunctionDefinitionBase& add_constant_evaluator_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "const",
        &language::plugins::construct_shared<
            TaskDependentFactory<Evaluator>,
            ConstEvaluatorFactory,
            std::string,
            Verbosity,
            int>);
    f.document_title("Constant evaluator");
    f.document_synopsis("Returns a constant value.");

    f.make_optional_argument_with_default(0, "value", "1", "the constant value");
    add_evaluator_options_to_feature(f, "const", 1);

    return f;
}

} // namespace

namespace downward::cli::evaluators {

void add_const_evaluator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    const auto& f = add_constant_evaluator_feature(n);

    DocumentationTopic& subcategory =
        registry.get_topic_by_name("evaluators_basic");
    subcategory.add_function(f);
}

} // namespace downward::cli::evaluators
