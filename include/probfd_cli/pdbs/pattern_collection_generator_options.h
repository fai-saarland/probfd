#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_H

#include "probfd_cli/pdbs/pattern_collection_generator_options_fwd.h"

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace probfd::cli::pdbs {

extern void add_pattern_collection_generator_options_to_feature(
    language::plugins::Feature& feature);

extern PatternCollectionGeneratorArgs
get_pattern_collection_generator_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace probfd::cli::pdbs

#endif // PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_H
