#include "downward/cli/heuristics/cea_heuristic_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/heuristics/cea_heuristic.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::cea_heuristic;
using namespace downward::utils;

using namespace language::parser;

using downward::cli::add_heuristic_options_to_feature;

namespace {
class ContextEnhancedAdditiveHeuristicFactory
    : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    Verbosity verbosity;

public:
    ContextEnhancedAdditiveHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        Verbosity verbosity)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);
        return std::make_unique<ContextEnhancedAdditiveHeuristic>(
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

} // namespace

namespace downward::cli::heuristics {

InternalFunctionDefinitionBase&
add_cea_heuristic_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskDependentFactory<Evaluator>,
        ContextEnhancedAdditiveHeuristicFactory,
        shared_ptr<TaskTransformation>,
        bool,
        string,
        Verbosity>>(nspace, "cea");

    f.document_title("Context-enhanced additive heuristic");

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "supported");
    f.document_language_support(
        "axioms",
        "supported (in the sense that the planner won't complain -- "
        "handling of axioms might be very stupid "
        "and even render the heuristic unsafe)");

    f.document_property("admissible", "no");
    f.document_property("consistent", "no");
    f.document_property("safe", "no");
    f.document_property("preferred operators", "yes");

    add_heuristic_options_to_feature(f, "cea", 0);

    return f;
}

} // namespace downward::cli::heuristics
