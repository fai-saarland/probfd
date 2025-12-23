#ifndef PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_ALIVE_H
#define PROBFD_CLI_MERGE_AND_SHRINK_PRUNE_STRATEGY_ALIVE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace probfd::cli::merge_and_shrink {

language::plugins::InternalFunctionDefinitionBase&
add_prune_strategy_alive_feature(language::plugins::Namespace& nspace);

}

#endif