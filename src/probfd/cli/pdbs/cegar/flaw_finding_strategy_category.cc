#include "probfd/cli/pdbs/cegar/flaw_finding_strategy_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

using namespace probfd::pdbs::cegar;

using namespace downward::cli::plugins;

namespace probfd::cli::pdbs::cegar {

void add_flaw_finding_strategy_category(Registry& raw_registry)
{
    raw_registry.insert_shared_category_plugin<FlawFindingStrategy>(
        "FlawFindingStrategy",
        "Policy CEGAR flaw finding strategy.");
}

} // namespace probfd::cli::pdbs::cegar
