#ifndef DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_CATEGORY_H
#define DOWNWARD_CLI_PDBS_PATTERN_GENERATOR_CATEGORY_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::pdbs {

void add_pattern_generator_category(
    language::plugins::Registry& registry);

}

#endif