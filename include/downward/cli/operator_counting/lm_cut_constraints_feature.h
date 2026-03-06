#ifndef DOWNWARD_CLI_OPERATOR_COUNTING_LM_CUT_CONSTRAINTS_FEATURE_H
#define DOWNWARD_CLI_OPERATOR_COUNTING_LM_CUT_CONSTRAINTS_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::operator_counting {

language::parser::InternalFunctionDefinitionBase&
add_lm_cut_constraints_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif