#ifndef PROBFD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_H
#define PROBFD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_H

#include "downward/utils/logging.h"

// Forward Declarations
namespace plugins {
class Feature;
class Options;
} // namespace plugins

namespace probfd_plugins::pdbs {

extern void
add_pattern_collection_generator_options_to_feature(plugins::Feature& feature);

extern std::tuple<utils::Verbosity>
get_collection_generator_arguments_from_options(const plugins::Options& opts);

} // namespace probfd_plugins::pdbs

#endif
