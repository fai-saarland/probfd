#ifndef PROBFD_CLI_SOLVERS_ACYCLIC_VI_H
#define PROBFD_CLI_SOLVERS_ACYCLIC_VI_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace probfd::cli::solvers {

language::parser::InternalFunctionDefinitionBase&
add_acyclic_value_iteration_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif