#ifndef DOWNWARD_PLUGINS_PDBS_CEGAR_H
#define DOWNWARD_PLUGINS_PDBS_CEGAR_H

#include <cstddef>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
} // namespace downward::cli::plugins

namespace downward::cli::pdbs {

extern void
add_cegar_implementation_notes_to_feature(plugins::InternalFunctionDefinitionBase& feature);

extern std::size_t add_cegar_wildcard_option_to_feature(
    plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::pdbs

#endif
