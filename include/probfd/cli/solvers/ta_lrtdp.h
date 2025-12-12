#ifndef PROBFD_CLI_SOLVERS_TA_LRTDP_H
#define PROBFD_CLI_SOLVERS_TA_LRTDP_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_ta_lrtdp_feature(
    language::plugins::Registry& registry);

}

#endif