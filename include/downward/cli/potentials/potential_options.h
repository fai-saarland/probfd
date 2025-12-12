#ifndef CLI_POTENTIALS_POTENTIAL_OPTIONS_H
#define CLI_POTENTIALS_POTENTIAL_OPTIONS_H

#include <cstddef>
#include <string>

namespace language::plugins {
class InternalFunctionDefinitionBase;
} // namespace language::plugins

namespace downward::cli::potentials {

std::string get_admissible_potentials_reference();

std::size_t add_admissible_potentials_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    const std::string& description,
    std::size_t start_index);

} // namespace downward::cli::potentials

#endif // CLI_POTENTIALS_POTENTIAL_OPTIONS_H
