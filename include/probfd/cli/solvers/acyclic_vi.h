#ifndef PROBFD_CLI_SOLVERS_ACYCLIC_VI_H
#define PROBFD_CLI_SOLVERS_ACYCLIC_VI_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace probfd::cli::solvers {

language::plugins::InternalFunctionDefinitionBase&
add_acyclic_value_iteration_feature(language::plugins::Namespace& nspace);

}

#endif