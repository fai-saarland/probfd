#ifndef PROBFD_PLUGINS_PDBS_CEGAR_CEGAR_H
#define PROBFD_PLUGINS_PDBS_CEGAR_CEGAR_H

#include <tuple>

// Forward Declarations
namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace probfd::cli::pdbs {

extern void
add_cegar_wildcard_option_to_feature(downward::cli::plugins::Feature& feature);

extern std::tuple<bool> get_cegar_wildcard_arguments_from_options(
    const downward::cli::plugins::Options& opts);

} // namespace probfd::cli::pdbs

#endif
