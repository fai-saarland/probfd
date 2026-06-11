#ifndef DOWNWARD_CLI_LP_LP_SOLVER_ENUM_H
#define DOWNWARD_CLI_LP_LP_SOLVER_ENUM_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::lp {

void add_lp_solver_enum(
    language::plugins::RawRegistry& raw_registry);

}

#endif