#ifndef PROBFD_CLI_PDBS_CEGAR_PUCS_FLAW_FINDER_H
#define PROBFD_CLI_PDBS_CEGAR_PUCS_FLAW_FINDER_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace probfd::cli::pdbs::cegar {

language::plugins::InternalFunctionDefinitionBase&
add_pucs_flaw_finder_feature(language::plugins::Namespace& nspace);

}

#endif