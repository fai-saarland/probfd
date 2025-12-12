#ifndef PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_CATEGORY_H
#define PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_CATEGORY_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::merge_and_shrink {

void add_shrink_strategy_category(
    language::plugins::Registry& registry);

}

#endif