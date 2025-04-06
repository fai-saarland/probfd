#ifndef PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_OPTIONS_FWD_H
#define PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_OPTIONS_FWD_H

#include "probfd/cli/pdbs/pattern_collection_generator_options_fwd.h"

#include "probfd/aliases.h"

#include <memory>
#include <utility>

namespace downward::utils {
enum class Verbosity;
class RandomNumberGenerator;
} // namespace utils

namespace probfd::cli::pdbs {

using PatternCollectionGeneratorMultipleAdditionalArgs =
    std::tuple<int, int, double, double, double, double, bool, bool>;

using PatternCollectionGeneratorMultipleArgs = TupleCatType<
    PatternCollectionGeneratorMultipleAdditionalArgs,
    std::tuple<std::shared_ptr<downward::utils::RandomNumberGenerator>>,
    PatternCollectionGeneratorArgs>;

} // namespace probfd::cli::pdbs

#endif // PROBFD_CLI_PDBS_PATTERN_COLLECTION_GENERATOR_MULTIPLE_OPTIONS_FWD_H
