#ifndef DOWNWARD_PLUGINS_PDBS_PATTERN_GENERATOR_H
#define DOWNWARD_PLUGINS_PDBS_PATTERN_GENERATOR_H

#include <cstddef>

namespace language::plugins {
class InternalFunctionDefinitionBase;
} // namespace language::plugins

namespace downward::cli::pdbs {

extern std::size_t add_generator_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::pdbs

#endif
