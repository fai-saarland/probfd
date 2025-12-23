#ifndef DOWNWARD_CLI_OPERATOR_COUNTING_DELETE_RELAXATION_RR_CONSTRAINTS_FEATURE_H
#define DOWNWARD_CLI_OPERATOR_COUNTING_DELETE_RELAXATION_RR_CONSTRAINTS_FEATURE_H

namespace language::plugins {
class Namespace;
}

namespace downward::cli::operator_counting {

void add_delete_relaxation_rr_constraints_feature(
    language::plugins::Namespace& nspace);

} // namespace downward::cli::operator_counting

#endif