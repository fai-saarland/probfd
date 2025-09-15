#ifndef DOWNWARD_CLI_OPERATOR_COUNTING_DELETE_RELAXATION_IF_CONSTRAINTS_FEATURE_H
#define DOWNWARD_CLI_OPERATOR_COUNTING_DELETE_RELAXATION_IF_CONSTRAINTS_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::operator_counting {

void add_delete_relaxation_if_constraints_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif