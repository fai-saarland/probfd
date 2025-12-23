#ifndef PROBFD_CLI_PDBS_CEGAR_FLAW_FINDER_STRATEGY_H
#define PROBFD_CLI_PDBS_CEGAR_FLAW_FINDER_STRATEGY_H

namespace language::plugins {
class Namespace;
}

namespace probfd::cli::pdbs::cegar {

void add_flaw_finding_strategy_category(language::plugins::Namespace& nspace);

}

#endif