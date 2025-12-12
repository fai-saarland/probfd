#ifndef DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SELECTOR_CATEGORY_H
#define DOWNWARD_CLI_MERGE_AND_SHRINK_MERGE_SELECTOR_CATEGORY_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::merge_and_shrink {

void add_merge_selector_category(
    language::plugins::Registry& registry);

}

#endif