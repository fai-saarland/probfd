#ifndef DOWNWARD_PLUGINS_OPEN_LIST_FACTORY_H
#define DOWNWARD_PLUGINS_OPEN_LIST_FACTORY_H

#include <tuple>

namespace downward::cli::plugins {
class Options;
class Feature;
} // namespace downward::cli::plugins

namespace downward::cli {

extern void add_open_list_options_to_feature(plugins::Feature& feature);

extern std::tuple<bool>
get_open_list_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli

#endif
