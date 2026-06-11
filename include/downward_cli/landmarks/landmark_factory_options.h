#ifndef DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_FACTORY_H
#define DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_FACTORY_H

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

namespace downward::cli::landmarks {

extern void
add_landmark_factory_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<utils::Verbosity> get_landmark_factory_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

extern void
add_use_orders_option_to_feature(language::plugins::Feature& feature);

extern bool get_use_orders_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli::landmarks

#endif
