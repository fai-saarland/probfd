#ifndef DOWNWARD_PLUGINS_EVALUATORS_COMBINING_EVALUATOR_H
#define DOWNWARD_PLUGINS_EVALUATORS_COMBINING_EVALUATOR_H

#include <memory>
#include <string>
#include <vector>

namespace downward {
class Evaluator;
}

namespace downward::utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::cli::combining_evaluator {

extern void add_combining_evaluator_options_to_feature(
    plugins::Feature& feature,
    const std::string& description);

extern std::tuple<
    std::vector<std::shared_ptr<Evaluator>>,
    const std::string,
    utils::Verbosity>
get_combining_evaluator_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli::combining_evaluator

#endif
