#ifndef DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_FACTORY_H
#define DOWNWARD_PLUGINS_LANDMARKS_LANDMARK_FACTORY_H

#include <tuple>

namespace utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::cli::landmarks {

extern void add_landmark_factory_options_to_feature(plugins::Feature& feature);
extern std::tuple<utils::Verbosity>
get_landmark_factory_arguments_from_options(const plugins::Options& opts);

extern void add_use_orders_option_to_feature(plugins::Feature& feature);
extern bool get_use_orders_arguments_from_options(const plugins::Options& opts);

extern void
add_only_causal_landmarks_option_to_feature(plugins::Feature& feature);
extern bool
get_only_causal_landmarks_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli::landmarks

#endif
