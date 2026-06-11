#ifndef DOWNWARD_PLUGINS_OPEN_LIST_FACTORY_H
#define DOWNWARD_PLUGINS_OPEN_LIST_FACTORY_H

#include <tuple>

namespace language {
class Context;
}

namespace language::plugins {
class Options;
class Feature;
} // namespace language::plugins

namespace downward::cli {

extern void
add_open_list_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<bool> get_open_list_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli

#endif
