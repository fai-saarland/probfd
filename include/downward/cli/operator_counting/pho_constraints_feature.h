#ifndef DOWNWARD_CLI_OPERATOR_COUNTING_PHO_CONSTRAINTS_FEATURE_H
#define DOWNWARD_CLI_OPERATOR_COUNTING_PHO_CONSTRAINTS_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::operator_counting {

void add_pho_constraints_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif