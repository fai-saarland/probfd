#ifndef PROBFD_CLI_HEURISTICS_SCP_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_SCP_HEURISTIC_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::heuristics {

void add_scp_heuristic_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif