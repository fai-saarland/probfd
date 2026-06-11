#ifndef DOWNWARD_CLI_UTILS_VERBOSITY_ENUM_H
#define DOWNWARD_CLI_UTILS_VERBOSITY_ENUM_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::utils {

void add_verbosity_enum(
    language::plugins::RawRegistry& raw_registry);

}

#endif