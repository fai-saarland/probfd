#ifndef DOWNWARD_CLI_UTILS_VERBOSITY_ENUM_H
#define DOWNWARD_CLI_UTILS_VERBOSITY_ENUM_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::utils {

void add_verbosity_enum(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif