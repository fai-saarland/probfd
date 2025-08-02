#ifndef PROBFD_CLI_PDBS_CEGAR_OPTIONS_H
#define PROBFD_CLI_PDBS_CEGAR_OPTIONS_H

#include "cegar_options_fwd.h"

// Forward Declarations
namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace probfd::cli::pdbs {

extern void
add_cegar_wildcard_option_to_feature(downward::cli::plugins::Feature& feature);

extern CEGARArgs get_cegar_wildcard_arguments_from_options(
    const downward::cli::plugins::Options& opts);

} // namespace probfd::cli::pdbs

#endif // PROBFD_CLI_PDBS_CEGAR_OPTIONS_H
