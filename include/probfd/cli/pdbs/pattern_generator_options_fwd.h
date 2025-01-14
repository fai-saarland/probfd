#ifndef PROBFD_CLI_PDBS_PATTERN_GENERATOR_OPTIONS_FWD_H
#define PROBFD_CLI_PDBS_PATTERN_GENERATOR_OPTIONS_FWD_H

#include <tuple>

// Forward Declarations
namespace utils {
enum class Verbosity;
}

namespace probfd::cli::pdbs {
using PatternGeneratorArgs = std::tuple<utils::Verbosity>;
} // namespace probfd::cli::pdbs

#endif
