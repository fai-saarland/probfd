#ifndef DOWNWARD_CLI_OPERATOR_COUNTING_STATE_EQUATION_CONSTRAINTS_FEATURE_H
#define DOWNWARD_CLI_OPERATOR_COUNTING_STATE_EQUATION_CONSTRAINTS_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::operator_counting {

language::parser::InternalFunctionDefinitionBase&
add_state_equation_constraints_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif