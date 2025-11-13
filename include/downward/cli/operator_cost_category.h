#ifndef DOWNWARD_CLI_OPERATOR_COST_CATEGORY_H
#define DOWNWARD_CLI_OPERATOR_COST_CATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli {

void add_operator_cost_category(
    downward::cli::plugins::Registry& registry);

}

#endif