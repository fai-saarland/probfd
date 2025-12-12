#ifndef DOWNWARD_PLUGINS_PDBS_CANONICAL_PDBS_HEURISTIC_H
#define DOWNWARD_PLUGINS_PDBS_CANONICAL_PDBS_HEURISTIC_H

#include <cstddef>

namespace language::plugins {
class InternalFunctionDefinitionBase;
} // namespace language::plugins

namespace downward::cli::pdbs {

extern std::size_t add_canonical_pdbs_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::pdbs

#endif
