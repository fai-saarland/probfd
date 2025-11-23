#include "downward/cli/heuristics/canonical_pdbs_heuristic_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/heuristics/heuristic_options.h"

#include "downward/cli/pdbs/canonical_pdbs_heuristic_options.h"

#include "downward/pdbs/canonical_pdbs_heuristic.h"

#include "downward/task_dependent_factory.h"
#include "downward/task_transformation.h"

using namespace std;
using namespace downward;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;

using downward::cli::pdbs::add_canonical_pdbs_options_to_feature;

namespace {
class CanonicalPDBsHeuristicFactory : public TaskDependentFactory<Evaluator> {
    std::shared_ptr<TaskTransformation> transformation;
    bool cache_estimates;
    std::string description;
    utils::Verbosity verbosity;
    std::shared_ptr<PatternCollectionGenerator> generator;
    FSeconds max_time_dominance_pruning;

public:
    CanonicalPDBsHeuristicFactory(
        shared_ptr<TaskTransformation> transformation,
        bool cache_estimates,
        string description,
        utils::Verbosity verbosity,
        std::shared_ptr<PatternCollectionGenerator> generator,
        FSeconds max_time_dominance_pruning)
        : transformation(std::move(transformation))
        , cache_estimates(cache_estimates)
        , description(std::move(description))
        , verbosity(verbosity)
        , generator(std::move(generator))
        , max_time_dominance_pruning(max_time_dominance_pruning)
    {
    }

    unique_ptr<Evaluator> create_object(const SharedAbstractTask& task) override
    {
        auto transformation_result = transformation->transform(task);

        return std::make_unique<CanonicalPDBsHeuristic>(
            std::move(generator),
            max_time_dominance_pruning,
            task,
            std::move(transformation_result),
            cache_estimates,
            description,
            verbosity);
    }
};

InternalFunctionDefinitionBase& add_canonical_pdb_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "cpdbs",
        &downward::cli::plugins::construct_shared<
            TaskDependentFactory<Evaluator>,
            CanonicalPDBsHeuristicFactory,
            shared_ptr<TaskTransformation>,
            bool,
            string,
            Verbosity,
            std::shared_ptr<PatternCollectionGenerator>,
            FSeconds>);

    f.document_title("Canonical PDB");
    f.document_synopsis(
        "The canonical pattern database heuristic is calculated as "
        "follows. "
        "For a given pattern collection C, the value of the "
        "canonical heuristic function is the maximum over all "
        "maximal additive subsets A in C, where the value for one subset "
        "S in A is the sum of the heuristic values for all patterns in S "
        "for a given state.");

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
    const auto n = add_canonical_pdbs_options_to_feature(f, 1);
    add_heuristic_options_to_feature(f, "cpdbs", n + 1);

    return f;
}

} // namespace

namespace downward::cli::heuristics {

void add_canonical_pdbs_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    const InternalFunctionDefinitionBase& f = add_canonical_pdb_heuristic_to_namespace(n);
    DocumentationTopic& subcategory =
        registry.get_topic_by_name("heuristics_pdb");
    subcategory.add_function(f);
}

} // namespace downward::cli::heuristics
