#ifndef PROBFD_PLUGINS_PDBS_PATTERN_GENERATOR_H
#define PROBFD_PLUGINS_PDBS_PATTERN_GENERATOR_H

#include "downward/utils/logging.h"

#include <utility>

// Forward Declarations
namespace plugins {
class Feature;
class Options;
} // namespace plugins

namespace probfd_plugins::pdbs {

extern void add_pattern_generator_options_to_feature(plugins::Feature& feature);

std::tuple<utils::Verbosity>
get_generator_arguments_from_options(const plugins::Options& opts);

} // namespace probfd_plugins::pdbs

#endif
