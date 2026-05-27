#ifndef DOWNWARD_CLI_OPERATOR_COUNTING_CONSTRAINT_GENERATOR_CATEGORY_H
#define DOWNWARD_CLI_OPERATOR_COUNTING_CONSTRAINT_GENERATOR_CATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::operator_counting {

void add_constraint_generator_category(
    downward::cli::plugins::Registry& registry);

}

#endif