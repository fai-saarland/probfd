#include "downward/cli/cartesian_abstractions/subtask_generators_category.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cartesian_abstractions/subtask_generators.h"

using namespace downward::cartesian_abstractions;

using namespace language::plugins;

namespace downward::cli::cartesian_abstractions {

void add_subtask_generator_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<SubtaskGenerator>(
        "SubtaskGenerator",
        "Subtask generator (used by the CEGAR heuristic).");

    nspace.insert_enum_declaration<FactOrder>(
        {{"original", "according to their (internal) variable index"},
         {"random", "according to a random permutation"},
         {"hadd_up", "according to their h^add value, lowest first"},
         {"hadd_down", "according to their h^add value, highest first "}});
}

} // namespace downward::cli::cartesian_abstractions
