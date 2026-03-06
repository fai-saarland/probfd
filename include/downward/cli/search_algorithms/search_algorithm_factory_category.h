#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_SEARCH_ALGORITHM_FACTORY_CATEGORY_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_SEARCH_ALGORITHM_FACTORY_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::search_algorithms {

void add_search_algorithm_factory_category(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif