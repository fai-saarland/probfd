#ifndef PROBFD_CLI_TRANSITION_SORTERS_SUBCATEGORY_H
#define PROBFD_CLI_TRANSITION_SORTERS_SUBCATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::transiton_sorters {

void add_transition_sorter_category(language::parser::NamespaceLevelDeclarationList& nspace);

void add_transition_sorter_features(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace probfd::cli::transiton_sorters

#endif