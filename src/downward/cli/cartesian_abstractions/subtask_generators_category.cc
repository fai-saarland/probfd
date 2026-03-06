#include "downward/cli/cartesian_abstractions/subtask_generators_category.h"

#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_type_declaration.h"

#include "downward/cartesian_abstractions/subtask_generators.h"

using namespace downward::cartesian_abstractions;

using namespace language::parser;

namespace downward::cli::cartesian_abstractions {

void add_subtask_generator_category(NamespaceLevelDeclarationList& list)
{
    insert_shared_type_declaration<SubtaskGenerator>(
        list,
        "SubtaskGenerator",
        "Subtask generator (used by the CEGAR heuristic).");

    insert_enum_declaration<FactOrder>(
        list,
        "FactOrder",
        {{"original", "according to their (internal) variable index"},
         {"random", "according to a random permutation"},
         {"hadd_up", "according to their h^add value, lowest first"},
         {"hadd_down", "according to their h^add value, highest first "}});
}

} // namespace downward::cli::cartesian_abstractions
