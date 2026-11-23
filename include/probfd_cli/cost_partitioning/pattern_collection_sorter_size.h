#ifndef PROBFD_CLI_COST_PARTITIONING_PATTERN_COLLECTION_SORTER_SIZE_H
#define PROBFD_CLI_COST_PARTITIONING_PATTERN_COLLECTION_SORTER_SIZE_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::cost_partitioning {

void add_pattern_collection_sorter_size_features(
    language::plugins::RawRegistry& raw_registry);

} // namespace probfd::cli::cost_partitioning

#endif