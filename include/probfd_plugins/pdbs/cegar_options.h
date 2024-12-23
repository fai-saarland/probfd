#ifndef PROBFD_PLUGINS_PDBS_CEGAR_CEGAR_H
#define PROBFD_PLUGINS_PDBS_CEGAR_CEGAR_H

#include <tuple>

// Forward Declarations
namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace probfd_plugins::pdbs {

extern void add_cegar_wildcard_option_to_feature(
    downward_plugins::plugins::Feature& feature);

extern std::tuple<bool> get_cegar_wildcard_arguments_from_options(
    const downward_plugins::plugins::Options& opts);

} // namespace probfd_plugins::pdbs

#endif
