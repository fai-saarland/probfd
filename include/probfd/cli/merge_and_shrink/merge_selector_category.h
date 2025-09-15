#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SELECTOR_CATEGORY_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_SELECTOR_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::merge_and_shrink {

void add_merge_selector_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif