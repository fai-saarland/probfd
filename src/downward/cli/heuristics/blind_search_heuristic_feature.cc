#include "downward/cli/heuristics/blind_search_heuristic_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/heuristics/blind_search_heuristic.h"

#include "downward/utils/logging.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::blind_search_heuristic;

using namespace language::parser;

using downward::cli::add_heuristic_options_to_feature;

namespace {

class BlindSearchHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    Verbosity verbosity;

public:
    BlindSearchHeuristicFactory(
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
        return std::make_unique<BlindSearchHeuristic>(
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
add_blind_heuristic_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskDependentFactory<Evaluator>,
        BlindSearchHeuristicFactory,
        shared_ptr<TaskTransformation>,
        bool,
        string,
        Verbosity>>(nspace, "blind");

    f.document_title("Blind heuristic");
    f.document_synopsis(
        "Returns cost of cheapest action for non-goal states, "
        "0 for goal states");

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "supported");
    f.document_language_support("axioms", "supported");

    f.document_property("admissible", "yes");
    f.document_property("consistent", "yes");
    f.document_property("safe", "yes");
    f.document_property("preferred operators", "no");

    add_heuristic_options_to_feature(f, "blind", 0);

    return f;
}

} // namespace downward::cli::heuristics
