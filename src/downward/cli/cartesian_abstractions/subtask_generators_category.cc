#include "downward/cli/cartesian_abstractions/subtask_generators_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cartesian_abstractions/subtask_generators.h"

using namespace downward::cartesian_abstractions;

using namespace downward::cli::plugins;

namespace downward::cli::cartesian_abstractions {

void add_subtask_generator_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<SubtaskGenerator>(
        "SubtaskGenerator",
        "Subtask generator (used by the CEGAR heuristic).");

    n.insert_enum_declaration<FactOrder>(
        {{"original", "according to their (internal) variable index"},
         {"random", "according to a random permutation"},
         {"hadd_up", "according to their h^add value, lowest first"},
         {"hadd_down", "according to their h^add value, highest first "}});
}

} // namespace downward::cli::cartesian_abstractions
