#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_LAZY_FEATURE_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_LAZY_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::search_algorithms {

language::parser::InternalFunctionDefinitionBase&
add_lazy_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif