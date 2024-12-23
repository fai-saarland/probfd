#ifndef DOWNWARD_PLUGINS_SEARCH_ALGORITHM_H
#define DOWNWARD_PLUGINS_SEARCH_ALGORITHM_H

#include <memory>
#include <string>
#include <tuple>

enum OperatorCost : unsigned short;

namespace utils {
enum class Verbosity;
}

class PruningMethod;

namespace downward_plugins::plugins {
class Options;
class Feature;
} // namespace downward_plugins::plugins

namespace downward_plugins {

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

} // namespace downward_plugins

#endif
