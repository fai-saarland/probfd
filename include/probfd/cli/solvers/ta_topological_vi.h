#ifndef PROBFD_CLI_SOLVERS_TA_TOPOLOGICAL_VI_H
#define PROBFD_CLI_SOLVERS_TA_TOPOLOGICAL_VI_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_ta_topological_value_iteration_feature(
    language::plugins::Registry& registry);

}

#endif