#include "downward/cli/evaluators/g_evaluator_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/evaluators/evaluator_options.h"

#include "downward/evaluators/g_evaluator.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::g_evaluator;

using namespace downward::cli;
using namespace language::plugins;

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

InternalFunctionDefinitionBase& add_g_evaluator_feature_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "g",
        &language::plugins::construct_shared<
            TaskDependentFactory<Evaluator>,
            GEvaluatorFactory,
            std::string,
            Verbosity>);
    f.document_title("g-value evaluator");
    f.document_synopsis("Returns the g-value (path cost) of the search node.");

    add_evaluator_options_to_feature(f, "g", 0);

    return f;
}

} // namespace

namespace downward::cli::evaluators {

void add_g_evaluator_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    const InternalFunctionDefinitionBase& f = add_g_evaluator_feature_to_namespace(n);
    DocumentationTopic& subcategory =
        registry.get_topic_by_name("evaluators_basic");
    subcategory.add_function(f);
}

} // namespace downward::cli::evaluators
