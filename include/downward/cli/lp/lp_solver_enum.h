#ifndef DOWNWARD_CLI_LP_LP_SOLVER_ENUM_H
#define DOWNWARD_CLI_LP_LP_SOLVER_ENUM_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::lp {

void add_lp_solver_enum(
    language::plugins::Registry& registry);

}

#endif