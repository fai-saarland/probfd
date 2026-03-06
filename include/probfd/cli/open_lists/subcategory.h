#ifndef PROBFD_CLI_OPEN_LISTS_SUBCATEGORY_H
#define PROBFD_CLI_OPEN_LISTS_SUBCATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::open_lists {

void add_open_list_categories(language::parser::NamespaceLevelDeclarationList& nspace);

void add_open_list_features(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace probfd::cli::open_lists

#endif