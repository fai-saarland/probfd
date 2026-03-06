#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_ITERATED_SEARCH_FEATURE_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_ITERATED_SEARCH_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::search_algorithms {

language::parser::InternalFunctionDefinitionBase&
add_iterated_search_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif