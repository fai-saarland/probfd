#ifndef DOWNWARD_PLUGINS_OPEN_LIST_FACTORY_H
#define DOWNWARD_PLUGINS_OPEN_LIST_FACTORY_H

#include <tuple>

namespace downward_plugins::plugins {
class Options;
class Feature;
} // namespace downward_plugins::plugins

namespace downward_plugins {

extern void add_open_list_options_to_feature(plugins::Feature& feature);

extern std::tuple<bool>
get_open_list_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins

#endif
