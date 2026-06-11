#ifndef DOWNWARD_PLUGINS_EVALUATOR_H
#define DOWNWARD_PLUGINS_EVALUATOR_H

#include <string>
#include <tuple>

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

namespace downward::cli {

extern void add_evaluator_options_to_feature(
    language::plugins::Feature& feature,
    const std::string& description);

extern std::tuple<std::string, utils::Verbosity>
get_evaluator_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli

#endif
