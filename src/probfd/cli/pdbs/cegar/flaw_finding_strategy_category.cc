#include "probfd/cli/pdbs/cegar/flaw_finding_strategy_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

using namespace probfd::pdbs::cegar;

using namespace downward::cli::plugins;

namespace probfd::cli::pdbs::cegar {

void add_flaw_finding_strategy_category(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_shared_type_declaration<FlawFindingStrategy>(
        "FlawFindingStrategy",
        "Policy CEGAR flaw finding strategy.");
}

} // namespace probfd::cli::pdbs::cegar
