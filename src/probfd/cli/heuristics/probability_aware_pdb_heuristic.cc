#include "probfd/cli/heuristics/probability_aware_pdb_heuristic.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/heuristics/probability_aware_pdb_heuristic.h"

using namespace downward;
using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace language::plugins;

namespace {

InternalFunctionDefinitionBase& add_pattern_database_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "ppdbs",
        &language::plugins::construct_shared<
            TaskHeuristicFactory,
            ProbabilityAwarePDBHeuristicFactory,
            std::shared_ptr<PatternCollectionGenerator>,
            utils::FSeconds,
            utils::Verbosity>);

    f.document_title("Probability-aware Pattern database heuristic");
    f.document_synopsis(
        "An admissible heuristic obtained by combining multiple projection "
        "heuristics.");

    f.document_language_support("action costs", "supported");
    f.document_language_support("conditional effects", "not supported");
    f.document_language_support("axioms", "not supported");

    f.document_property("admissible", "yes");
    f.document_property("consistent", "yes");
    f.document_property("safe", "yes");

    f.make_optional_argument_with_default(
        0,
        "patterns",
        "classical_generator(generator=systematic(pattern_max_size=2))",
        "The pattern generation method");
    f.make_optional_argument_with_default(
        1,
        "max_time_dominance_pruning",
        "0.0s",
        "The maximum time spent pruning dominated patterns");
    add_task_dependent_heuristic_options_to_feature(f, 2);

    return f;
}

} // namespace

namespace probfd::cli::heuristics {

void add_pdb_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pattern_database_heuristic_to_namespace(n);
}

} // namespace probfd::cli::heuristics
