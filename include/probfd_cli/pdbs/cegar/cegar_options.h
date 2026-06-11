#ifndef PROBFD_CLI_PDBS_CEGAR_OPTIONS_H
#define PROBFD_CLI_PDBS_CEGAR_OPTIONS_H

#include "cegar_options_fwd.h"

// Forward Declarations
namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace probfd::cli::pdbs {

extern void
add_cegar_wildcard_option_to_feature(language::plugins::Feature& feature);

extern CEGARArgs get_cegar_wildcard_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace probfd::cli::pdbs

#endif // PROBFD_CLI_PDBS_CEGAR_OPTIONS_H
