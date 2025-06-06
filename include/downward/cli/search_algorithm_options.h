#ifndef DOWNWARD_PLUGINS_SEARCH_ALGORITHM_H
#define DOWNWARD_PLUGINS_SEARCH_ALGORITHM_H

#include <memory>
#include <string>
#include <tuple>

namespace downward {
enum OperatorCost : unsigned short;
class PruningMethod;
}

namespace downward::utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Options;
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli {

extern void add_search_pruning_options_to_feature(plugins::Feature& feature);

extern std::tuple<std::shared_ptr<PruningMethod>>
get_search_pruning_arguments_from_options(const plugins::Options& opts);

extern void add_search_algorithm_options_to_feature(
    plugins::Feature& feature,
    const std::string& description);

extern std::tuple<OperatorCost, int, double, std::string, utils::Verbosity>
get_search_algorithm_arguments_from_options(const plugins::Options& opts);

extern void add_successors_order_options_to_feature(plugins::Feature& feature);

extern std::tuple<bool, bool, int>
get_successors_order_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli

#endif
