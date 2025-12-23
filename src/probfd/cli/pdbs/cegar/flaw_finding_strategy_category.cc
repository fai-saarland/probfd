#include "probfd/cli/pdbs/cegar/flaw_finding_strategy_category.h"

#include "language/plugins/registry.h"

#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

using namespace probfd::pdbs::cegar;

using namespace language::plugins;

namespace probfd::cli::pdbs::cegar {

void add_flaw_finding_strategy_category(Namespace& nspace)
{
    nspace.insert_shared_type_declaration<FlawFindingStrategy>(
        "FlawFindingStrategy",
        "Policy CEGAR flaw finding strategy.");
}

} // namespace probfd::cli::pdbs::cegar
