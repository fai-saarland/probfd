#ifndef DOWNWARD_CLI_OPERATOR_COST_CATEGORY_H
#define DOWNWARD_CLI_OPERATOR_COST_CATEGORY_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli {

void add_operator_cost_category(language::plugins::RawRegistry& raw_registry);

}

#endif