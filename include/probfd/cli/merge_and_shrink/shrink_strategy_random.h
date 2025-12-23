#ifndef PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_RANDOM_H
#define PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_RANDOM_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace probfd::cli::merge_and_shrink {

language::plugins::InternalFunctionDefinitionBase&
add_shrink_strategy_random_feature(language::plugins::Namespace& nspace);

}

#endif