#ifndef DOWNWARD_CLI_PRUNING_STUBBORN_SETS_SIMPLE_FEATURE_H
#define DOWNWARD_CLI_PRUNING_STUBBORN_SETS_SIMPLE_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::pruning {

void add_stubborn_sets_simple_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif