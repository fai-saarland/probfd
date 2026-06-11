#ifndef DOWNWARD_PLUGINS_UTILS_LOGGING_H
#define DOWNWARD_PLUGINS_UTILS_LOGGING_H

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

namespace downward::cli::utils {

extern void add_log_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<downward::utils::Verbosity> get_log_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli::utils

#endif
