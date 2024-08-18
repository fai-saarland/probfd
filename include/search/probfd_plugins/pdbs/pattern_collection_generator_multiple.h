#ifndef PROBFD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
#define PROBFD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H

#include "downward/utils/logging.h"

#include <memory>
#include <utility>

// Forward Declarations
namespace plugins {
class Feature;
class Options;
} // namespace plugins

namespace utils {
class RandomNumberGenerator;
} // namespace utils

namespace probfd_plugins::pdbs {

extern void add_multiple_options_to_feature(plugins::Feature& feature);

extern std::tuple<
    int,
    int,
    double,
    double,
    double,
    double,
    bool,
    bool,
    std::shared_ptr<utils::RandomNumberGenerator>,
    utils::Verbosity>
get_multiple_arguments_from_options(const plugins::Options& options);

} // namespace probfd_plugins::pdbs

#endif
