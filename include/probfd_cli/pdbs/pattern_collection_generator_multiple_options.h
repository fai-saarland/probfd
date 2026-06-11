#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H

#include "probfd_cli/pdbs/pattern_collection_generator_multiple_options_fwd.h"

#include <memory>
#include <utility>

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
add_multiple_options_to_feature(language::plugins::Feature& feature);

extern PatternCollectionGeneratorMultipleArgs
get_multiple_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& options);

} // namespace probfd::cli::pdbs

#endif // PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
