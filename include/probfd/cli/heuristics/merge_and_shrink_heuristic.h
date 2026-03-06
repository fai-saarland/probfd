#ifndef PROBFD_CLI_HEURISTICS_MERGE_AND_SHRINK_HEURISTIC_H
#define PROBFD_CLI_HEURISTICS_MERGE_AND_SHRINK_HEURISTIC_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::heuristics {

void add_merge_and_shrink_heuristic_feature(language::parser::NamespaceLevelDeclarationList& n);

}

#endif