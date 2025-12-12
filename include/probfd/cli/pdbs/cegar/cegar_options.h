#ifndef PROBFD_CLI_PDBS_CEGAR_OPTIONS_H
#define PROBFD_CLI_PDBS_CEGAR_OPTIONS_H

#include <cstddef>

// Forward Declarations
namespace language::plugins {
class InternalFunctionDefinitionBase;
} // namespace language::plugins

namespace probfd::cli::pdbs {

extern std::size_t add_cegar_wildcard_option_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace probfd::cli::pdbs

#endif // PROBFD_CLI_PDBS_CEGAR_OPTIONS_H
