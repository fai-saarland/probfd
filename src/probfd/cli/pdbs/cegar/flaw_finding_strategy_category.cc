#include "probfd/cli/pdbs/cegar/flaw_finding_strategy_category.h"

#include "language/ast/internal_type_declaration.h"

#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

using namespace probfd::pdbs::cegar;

using namespace language::parser;

namespace probfd::cli::pdbs::cegar {

void add_flaw_finding_strategy_category(NamespaceLevelDeclarationList& nspace)
{
    insert_shared_type_declaration<FlawFindingStrategy>(
        nspace,
        "FlawFindingStrategy",
        "Policy CEGAR flaw finding strategy.");
}

} // namespace probfd::cli::pdbs::cegar
