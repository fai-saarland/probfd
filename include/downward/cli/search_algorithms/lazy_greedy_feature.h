#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_LAZY_GREEDY_FEATURE_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_LAZY_GREEDY_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace downward::cli::search_algorithms {

language::plugins::InternalFunctionDefinitionBase&
add_lazy_greedy_feature(language::plugins::Namespace& nspace);

}

#endif