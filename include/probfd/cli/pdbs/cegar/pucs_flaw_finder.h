#ifndef PROBFD_CLI_PDBS_CEGAR_PUCS_FLAW_FINDER_H
#define PROBFD_CLI_PDBS_CEGAR_PUCS_FLAW_FINDER_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::pdbs::cegar {

language::parser::InternalFunctionDefinitionBase&
add_pucs_flaw_finder_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif