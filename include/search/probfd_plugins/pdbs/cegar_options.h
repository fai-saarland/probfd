#ifndef PROBFD_PLUGINS_PDBS_CEGAR_CEGAR_H
#define PROBFD_PLUGINS_PDBS_CEGAR_CEGAR_H

#include <utility>

// Forward Declarations
namespace plugins {
class Feature;
class Options;
} // namespace plugins

namespace probfd_plugins::pdbs {

extern void add_cegar_wildcard_option_to_feature(plugins::Feature& feature);

extern std::tuple<bool>
get_cegar_wildcard_arguments_from_options(const plugins::Options& opts);

} // namespace probfd_plugins::pdbs

#endif
