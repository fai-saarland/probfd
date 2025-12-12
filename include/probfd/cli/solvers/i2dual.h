#ifndef PROBFD_CLI_SOLVERS_I2DUAL_H
#define PROBFD_CLI_SOLVERS_I2DUAL_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::solvers {

void add_i2dual_feature(
    language::plugins::Registry& registry);

}

#endif