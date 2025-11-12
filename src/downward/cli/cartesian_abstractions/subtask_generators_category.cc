#include "downward/cli/cartesian_abstractions/subtask_generators_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cartesian_abstractions/subtask_generators.h"

using namespace downward::cartesian_abstractions;

using namespace downward::cli::plugins;

namespace downward::cli::cartesian_abstractions {

void add_subtask_generator_category(Registry& raw_registry)
{
    raw_registry.insert_shared_category_plugin<SubtaskGenerator>(
        "SubtaskGenerator",
        "Subtask generator (used by the CEGAR heuristic).");

    raw_registry.insert_enum_plugin<FactOrder>(
        {{"original", "according to their (internal) variable index"},
         {"random", "according to a random permutation"},
         {"hadd_up", "according to their h^add value, lowest first"},
         {"hadd_down", "according to their h^add value, highest first "}});
}

} // namespace downward::cli::cartesian_abstractions
