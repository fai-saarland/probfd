#ifndef CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
#define CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H

#include "probfd/cli/pdbs/pattern_collection_generator_multiple_options_fwd.h"

#include <memory>
#include <utility>

// Forward Declarations
namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace probfd::cli::pdbs {

extern void
add_multiple_options_to_feature(downward::cli::plugins::Feature& feature);

extern PatternCollectionGeneratorMultipleArgs
get_multiple_arguments_from_options(
    const downward::cli::plugins::Options& options);

} // namespace probfd::cli::pdbs

#endif // CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
