#ifndef PROBFD_CLI_SUCCESSOR_SAMPLERS_SUBCATEGORY_H
#define PROBFD_CLI_SUCCESSOR_SAMPLERS_SUBCATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::successor_samplers {

void add_successor_sampler_category(language::parser::NamespaceLevelDeclarationList& nspace);

void add_successor_sampler_features(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace probfd::cli::successor_samplers

#endif