#ifndef DOWNWARD_CLI_PRUNING_PRUNING_METHOD_CATEGORY_H
#define DOWNWARD_CLI_PRUNING_PRUNING_METHOD_CATEGORY_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::pruning {

void add_pruning_method_category(
    language::plugins::Registry& registry);

}

#endif