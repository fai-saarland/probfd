#ifndef DOWNWARD_CLI_PRUNING_STUBBORN_SETS_SIMPLE_FEATURE_H
#define DOWNWARD_CLI_PRUNING_STUBBORN_SETS_SIMPLE_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::pruning {

language::plugins::InternalFunctionDefinitionBase&
add_stubborn_sets_simple_feature(language::plugins::Namespace& nspace);

}

#endif