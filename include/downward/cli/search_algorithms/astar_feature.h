#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_ASTAR_FEATURE_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_ASTAR_FEATURE_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::search_algorithms {

void add_astar_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif