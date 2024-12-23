#ifndef DOWNWARD_PLUGINS_EVALUATORS_COMBINING_EVALUATOR_H
#define DOWNWARD_PLUGINS_EVALUATORS_COMBINING_EVALUATOR_H

#include <memory>
#include <string>
#include <vector>

class Evaluator;

namespace utils {
enum class Verbosity;
}

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace downward_plugins::combining_evaluator {

extern void add_combining_evaluator_options_to_feature(
    plugins::Feature& feature,
    const std::string& description);

extern std::tuple<
    std::vector<std::shared_ptr<Evaluator>>,
    const std::string,
    utils::Verbosity>
get_combining_evaluator_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins::combining_evaluator

#endif
