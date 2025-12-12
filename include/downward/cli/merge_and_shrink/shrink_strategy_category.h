#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_CATEGORY_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_CATEGORY_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::merge_and_shrink {

void add_shrink_strategy_category(
    language::plugins::Registry& registry);

}

#endif