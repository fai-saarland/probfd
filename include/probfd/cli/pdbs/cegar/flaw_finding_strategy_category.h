#ifndef PROBFD_CLI_PDBS_CEGAR_FLAW_FINDER_STRATEGY_H
#define PROBFD_CLI_PDBS_CEGAR_FLAW_FINDER_STRATEGY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::pdbs::cegar {

void add_flaw_finding_strategy_category(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif