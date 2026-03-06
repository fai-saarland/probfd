#ifndef DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H
#define DOWNWARD_PLUGINS_UTILS_RNG_OPTIONS_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::utils {

extern void add_rng_type(language::parser::NamespaceLevelDeclarationList& nspace);

extern void add_default_rng_function(language::parser::NamespaceLevelDeclarationList& nspace);
extern void add_seeded_rng_function(language::parser::NamespaceLevelDeclarationList& nspace);

// Add random_seed option to feature.
extern std::size_t add_rng_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::utils

#endif
