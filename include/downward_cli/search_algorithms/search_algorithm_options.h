#ifndef DOWNWARD_PLUGINS_SEARCH_ALGORITHM_H
#define DOWNWARD_PLUGINS_SEARCH_ALGORITHM_H

#include "downward/utils/timer.h"

#include <memory>
#include <string>
#include <tuple>

namespace downward {
enum OperatorCost : unsigned short;
class PruningMethod;
} // namespace downward

namespace downward::utils {
enum class Verbosity;
}

namespace language {
class Context;
}

namespace language::plugins {
class Options;
class Feature;
} // namespace language::plugins

namespace downward::cli {

extern void
add_search_pruning_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<std::shared_ptr<PruningMethod>>
get_search_pruning_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

extern void add_search_algorithm_options_to_feature(
    language::plugins::Feature& feature,
    const std::string& description);

extern std::tuple<
    OperatorCost,
    int,
    downward::utils::Duration,
    std::string,
    utils::Verbosity>
get_search_algorithm_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

extern void
add_successors_order_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<bool, bool, int> get_successors_order_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli

#endif
