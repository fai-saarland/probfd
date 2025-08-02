#ifndef DOWNWARD_CLI_PRUNING_LIMITED_PRUNING_FEATURE_H
#define DOWNWARD_CLI_PRUNING_LIMITED_PRUNING_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::pruning {

void add_limited_pruning_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif