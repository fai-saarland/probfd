#ifndef DOWNWARD_CLI_LP_LP_SOLVER_ENUM_H
#define DOWNWARD_CLI_LP_LP_SOLVER_ENUM_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::lp {

void add_lp_solver_enum(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif