#ifndef DOWNWARD_PLUGINS_PDBS_CEGAR_H
#define DOWNWARD_PLUGINS_PDBS_CEGAR_H

#include <tuple>

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace downward_plugins::pdbs {

extern void
add_cegar_implementation_notes_to_feature(plugins::Feature& feature);

extern void add_cegar_wildcard_option_to_feature(plugins::Feature& feature);

std::tuple<bool>
get_cegar_wildcard_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins::pdbs

#endif
