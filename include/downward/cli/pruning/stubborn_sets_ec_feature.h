#ifndef DOWNWARD_CLI_PRUNING_STUBBORN_SETS_EC_FEATURE_H
#define DOWNWARD_CLI_PRUNING_STUBBORN_SETS_EC_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::pruning {

void add_stubborn_sets_ec_feature(
    downward::cli::plugins::Registry& raw_registry);

}

#endif