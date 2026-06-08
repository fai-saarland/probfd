#include "probfd/cli/heuristics/pdb_max_heuristic.h"

#include "probfd/heuristics/pdb_max_heuristic.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

using namespace downward;
using namespace probfd;
using namespace probfd::pdbs;
using namespace probfd::heuristics;

using namespace probfd::cli::heuristics;

using namespace language::plugins;

namespace {

InternalFunctionDefinitionBase& add_pdb_max_heuristic_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pdb_max",
        &language::plugins::construct_shared<
            TaskHeuristicFactory,
            PDBMaxHeuristicFactory,
            std::shared_ptr<PatternCollectionGenerator>,
            utils::Verbosity>);

    f.document_title("PDB Max heuristic");
    f.document_synopsis(
        "An admissible heuristic obtained by max'ing over multiple projection "
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
    add_task_dependent_heuristic_options_to_feature(f, 1);

    return f;
}

} // namespace

namespace probfd::cli::heuristics {

void add_pdb_max_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_pdb_max_heuristic_to_namespace(n);
}

} // namespace probfd::cli::heuristics
