#include "downward/cli/heuristics/zero_one_pdbs_heuristic_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/pdbs/zero_one_pdbs_heuristic.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace language::plugins;

using downward::cli::add_heuristic_options_to_feature;

namespace {
class ZOPDBsHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    std::shared_ptr<PatternCollectionGenerator> generator;

public:
    ZOPDBsHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        std::shared_ptr<PatternCollectionGenerator> generator)
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

        return std::make_unique<ZeroOnePDBsHeuristic>(
            std::move(generator),
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

InternalFunctionDefinitionBase& add_zero_one_pdbs_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "zopdbs",
        &language::plugins::construct_shared<
            TaskDependentFactory<Evaluator>,
            ZOPDBsHeuristicFactory,
            shared_ptr<TaskTransformation>,
            bool,
            string,
            Verbosity,
            std::shared_ptr<PatternCollectionGenerator>>);

    f.document_title("Zero-One PDB");
    f.document_synopsis(
        "The zero/one pattern database heuristic is simply the sum of the "
        "heuristic values of all patterns in the pattern collection. In "
        "contrast "
        "to the canonical pattern database heuristic, there is no need to "
        "check "
        "for additive subsets, because the additivity of the patterns is "
        "guaranteed by action cost partitioning. This heuristic uses the "
        "most "
        "simple form of action cost partitioning, i.e. if an operator "
        "affects "
        "more than one pattern in the collection, its costs are entirely "
        "taken "
        "into account for one pattern (the first one which it affects) and "
        "set "
        "to zero for all other affected patterns.");

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "not supported");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes");
    f.document_property("consistent", "yes");
    f.document_property("safe", "yes");
    f.document_property("preferred operators", "no");

    f.make_optional_argument_with_default(
        0,
        "patterns",
        "systematic(1)",
        "pattern generation method");
    add_heuristic_options_to_feature(f, "zopdbs", 1);

    return f;
}

} // namespace

namespace downward::cli::heuristics {

void add_zero_one_pdbs_heuristic_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    const InternalFunctionDefinitionBase& f = add_zero_one_pdbs_heuristic_to_namespace(n);
    DocumentationTopic& subcategory =
        registry.get_topic_by_name("heuristics_pdb");
    subcategory.add_function(f);
}

} // namespace downward::cli::heuristics
