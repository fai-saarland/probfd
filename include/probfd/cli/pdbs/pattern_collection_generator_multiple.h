#ifndef PROBFD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H
#define PROBFD_PLUGINS_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_H

#include <memory>
#include <utility>

// Forward Declarations
namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace utils {
enum class Verbosity;
class RandomNumberGenerator;
} // namespace utils

namespace probfd::cli::pdbs {

extern void
add_multiple_options_to_feature(downward::cli::plugins::Feature& feature);

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
get_multiple_arguments_from_options(
    const downward::cli::plugins::Options& options);

} // namespace probfd::cli::pdbs

#endif
