#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_EAGER_WASTAR_FEATURE_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_EAGER_WASTAR_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::search_algorithms {

language::plugins::InternalFunctionDefinitionBase&
add_eager_wastar_feature(language::plugins::Namespace& nspace);

}

#endif