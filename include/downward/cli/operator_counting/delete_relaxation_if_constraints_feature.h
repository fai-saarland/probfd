#ifndef DOWNWARD_CLI_OPERATOR_COUNTING_DELETE_RELAXATION_IF_CONSTRAINTS_FEATURE_H
#define DOWNWARD_CLI_OPERATOR_COUNTING_DELETE_RELAXATION_IF_CONSTRAINTS_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::operator_counting {

language::plugins::InternalFunctionDefinitionBase&
add_delete_relaxation_if_constraints_feature(
    language::plugins::Namespace& nspace);

}

#endif