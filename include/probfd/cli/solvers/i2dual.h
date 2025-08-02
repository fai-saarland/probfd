#ifndef PROBFD_CLI_SOLVERS_I2DUAL_H
#define PROBFD_CLI_SOLVERS_I2DUAL_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::solvers {

void add_i2dual_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif