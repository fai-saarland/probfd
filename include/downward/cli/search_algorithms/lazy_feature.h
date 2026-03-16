#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_LAZY_FEATURE_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_LAZY_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::search_algorithms {

language::plugins::InternalFunctionDefinitionBase&
add_lazy_feature(language::plugins::Namespace& nspace);

}

#endif