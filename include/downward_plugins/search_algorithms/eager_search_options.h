#ifndef DOWNWARD_PLUGINS_SEARCH_ALGORITHMS_EAGER_SEARCH_H
#define DOWNWARD_PLUGINS_SEARCH_ALGORITHMS_EAGER_SEARCH_H

#include <memory>
#include <string>
#include <tuple>

enum OperatorCost : unsigned short;

class PruningMethod;
class Evaluator;

namespace utils {
enum class Verbosity;
}

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace downward_plugins::eager_search {

extern void add_eager_search_options_to_feature(
    plugins::Feature& feature,
    const std::string& description);

extern std::tuple<
    std::shared_ptr<PruningMethod>,
    std::shared_ptr<Evaluator>,
    OperatorCost,
    int,
    double,
    std::string,
    utils::Verbosity>
get_eager_search_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins::eager_search

#endif
