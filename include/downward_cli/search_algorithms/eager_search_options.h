#ifndef DOWNWARD_PLUGINS_SEARCH_ALGORITHMS_EAGER_SEARCH_H
#define DOWNWARD_PLUGINS_SEARCH_ALGORITHMS_EAGER_SEARCH_H

#include "downward/utils/timer.h"

#include <memory>
#include <string>
#include <tuple>

namespace downward {
enum OperatorCost : unsigned short;

class PruningMethod;
class Evaluator;
} // namespace downward

namespace downward::utils {
enum class Verbosity;
}

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace downward::cli::eager_search {

extern void add_eager_search_options_to_feature(
    language::plugins::Feature& feature,
    const std::string& description);

extern std::tuple<
    std::shared_ptr<PruningMethod>,
    OperatorCost,
    int,
    downward::utils::Duration,
    std::string,
    utils::Verbosity>
get_eager_search_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli::eager_search

#endif
