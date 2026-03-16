#ifndef DOWNWARD_CLI_PRUNING_LIMITED_PRUNING_FEATURE_H
#define DOWNWARD_CLI_PRUNING_LIMITED_PRUNING_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::pruning {

language::plugins::InternalFunctionDefinitionBase&
add_limited_pruning_feature(language::plugins::Namespace& nspace);

}

#endif