#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_ITERATED_SEARCH_FEATURE_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_ITERATED_SEARCH_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::search_algorithms {

language::plugins::InternalFunctionDefinitionBase&
add_iterated_search_feature(language::plugins::Namespace& nspace);

}

#endif