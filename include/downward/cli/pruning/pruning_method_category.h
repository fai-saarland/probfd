#ifndef DOWNWARD_CLI_PRUNING_PRUNING_METHOD_CATEGORY_H
#define DOWNWARD_CLI_PRUNING_PRUNING_METHOD_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::pruning {

void add_pruning_method_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif