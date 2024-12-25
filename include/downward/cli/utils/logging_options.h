#ifndef DOWNWARD_PLUGINS_UTILS_LOGGING_H
#define DOWNWARD_PLUGINS_UTILS_LOGGING_H

#include <tuple>

namespace utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::cli::utils {

extern void add_log_options_to_feature(plugins::Feature& feature);
extern std::tuple<::utils::Verbosity>
get_log_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli::utils

#endif
