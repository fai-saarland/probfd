#ifndef DOWNWARD_CLI_SEARCH_ALGORITHMS_LAZY_WASTAR_FEATURE_H
#define DOWNWARD_CLI_SEARCH_ALGORITHMS_LAZY_WASTAR_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::search_algorithms {

void add_lazy_wastar_feature(
    language::plugins::Registry& registry);

}

#endif