#include "downward/cli/heuristics/pdb_heuristic_feature.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_function_definition.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/pdbs/pdb_heuristic.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace language::parser;

using downward::cli::add_heuristic_options_to_feature;

namespace {
class PDBHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    Verbosity verbosity;
    std::shared_ptr<PatternGenerator> generator;

public:
    PDBHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        Verbosity verbosity,
        std::shared_ptr<PatternGenerator> generator)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , generator(std::move(generator))
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);

        return std::make_unique<PDBHeuristic>(
            std::move(generator),
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
add_pdb_heuristic_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        TaskDependentFactory<Evaluator>,
        PDBHeuristicFactory,
        shared_ptr<TaskTransformation>,
        bool,
        string,
        Verbosity,
        std::shared_ptr<PatternGenerator>>>(nspace, "pdb");

    f.document_title("Pattern database heuristic");
    f.document_synopsis("TODO");

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "not supported");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes");
    f.document_property("consistent", "yes");
    f.document_property("safe", "yes");
    f.document_property("preferred operators", "no");

    f.make_optional_argument_with_default(
        0,
        "pattern",
        "greedy()",
        "pattern generation method");
    add_heuristic_options_to_feature(f, "pdb", 1);

    return f;
}

} // namespace downward::cli::heuristics
