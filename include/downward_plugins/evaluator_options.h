#ifndef DOWNWARD_PLUGINS_EVALUATOR_H
#define DOWNWARD_PLUGINS_EVALUATOR_H

#include <string>
#include <tuple>

namespace utils {
enum class Verbosity;
}

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace downward_plugins {

extern void add_evaluator_options_to_feature(
    plugins::Feature& feature,
    const std::string& description);

extern std::tuple<std::string, utils::Verbosity>
get_evaluator_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins

#endif
