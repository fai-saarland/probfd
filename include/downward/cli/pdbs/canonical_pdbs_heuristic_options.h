#ifndef DOWNWARD_PLUGINS_PDBS_CANONICAL_PDBS_HEURISTIC_H
#define DOWNWARD_PLUGINS_PDBS_CANONICAL_PDBS_HEURISTIC_H

#include <cstddef>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
} // namespace downward::cli::plugins

namespace downward::cli::pdbs {

extern std::size_t add_canonical_pdbs_options_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::pdbs

#endif
