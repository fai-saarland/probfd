
#ifndef PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_BUCKET_BASED_OPTIONS_H
#define PROBFD_CLI_MERGE_AND_SHRINK_SHRINK_STRATEGY_BUCKET_BASED_OPTIONS_H

#include <cstddef>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
}

namespace probfd::cli::merge_and_shrink {

extern std::size_t add_bucket_based_shrink_options_to_feature(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

}

#endif
