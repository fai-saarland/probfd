#ifndef DOWNWARD_CLI_OPERATOR_COST_CATEGORY_H
#define DOWNWARD_CLI_OPERATOR_COST_CATEGORY_H

namespace language::plugins {
class Registry;
}

namespace downward::cli {

void add_operator_cost_category(language::plugins::Registry& registry);

}

#endif