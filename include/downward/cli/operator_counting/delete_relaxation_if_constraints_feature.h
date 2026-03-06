#ifndef DOWNWARD_CLI_OPERATOR_COUNTING_DELETE_RELAXATION_IF_CONSTRAINTS_FEATURE_H
#define DOWNWARD_CLI_OPERATOR_COUNTING_DELETE_RELAXATION_IF_CONSTRAINTS_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::operator_counting {

language::parser::InternalFunctionDefinitionBase&
add_delete_relaxation_if_constraints_feature(
    language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif