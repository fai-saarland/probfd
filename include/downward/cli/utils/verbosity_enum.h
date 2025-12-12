#ifndef DOWNWARD_CLI_UTILS_VERBOSITY_ENUM_H
#define DOWNWARD_CLI_UTILS_VERBOSITY_ENUM_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::utils {

void add_verbosity_enum(
    language::plugins::Registry& registry);

}

#endif