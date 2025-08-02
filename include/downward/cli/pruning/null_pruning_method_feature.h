#ifndef DOWNWARD_CLI_PRUNING_NULL_PRUNING_METHOD_FEATURE_H
#define DOWNWARD_CLI_PRUNING_NULL_PRUNING_METHOD_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::pruning {

void add_null_pruning_method_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif