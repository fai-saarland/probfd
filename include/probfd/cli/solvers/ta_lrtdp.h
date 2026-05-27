#ifndef PROBFD_CLI_SOLVERS_TA_LRTDP_H
#define PROBFD_CLI_SOLVERS_TA_LRTDP_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_ta_lrtdp_feature(
    downward::cli::plugins::Registry& registry);

}

#endif