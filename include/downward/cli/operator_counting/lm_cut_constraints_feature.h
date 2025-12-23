#ifndef DOWNWARD_CLI_OPERATOR_COUNTING_LM_CUT_CONSTRAINTS_FEATURE_H
#define DOWNWARD_CLI_OPERATOR_COUNTING_LM_CUT_CONSTRAINTS_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace downward::cli::operator_counting {

language::plugins::InternalFunctionDefinitionBase&
add_lm_cut_constraints_feature(language::plugins::Namespace& nspace);

}

#endif