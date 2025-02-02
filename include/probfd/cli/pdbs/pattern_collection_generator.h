#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_H

#include "probfd/cli/pdbs/pattern_collection_generator_options_fwd.h"

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace probfd::cli::pdbs {

extern void add_pattern_collection_generator_options_to_feature(
    downward::cli::plugins::Feature& feature);

extern PatternCollectionGeneratorArgs
get_pattern_collection_generator_arguments_from_options(
    const downward::cli::plugins::Options& opts);

} // namespace probfd::cli::pdbs

#endif // PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_H
