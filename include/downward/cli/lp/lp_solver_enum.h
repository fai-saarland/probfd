#ifndef DOWNWARD_CLI_LP_LP_SOLVER_ENUM_H
#define DOWNWARD_CLI_LP_LP_SOLVER_ENUM_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::lp {

void add_lp_solver_enum(
    downward::cli::plugins::Registry& raw_registry);

}

#endif