#ifndef PROBFD_CLI_COST_PARTITIONING_TASK_PATTERN_COLLECTION_SORTER_FACTORY_CATEGORY_H
#define PROBFD_CLI_COST_PARTITIONING_TASK_PATTERN_COLLECTION_SORTER_FACTORY_CATEGORY_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::cost_partitioning {

void add_task_pattern_collection_sorter_factory_category(
    language::plugins::RawRegistry& raw_registry);

}

#endif