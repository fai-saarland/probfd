#ifndef DOWNWARD_CLI_OPEN_LISTS_TYPE_BASED_OPEN_LIST_FEATURE_H
#define DOWNWARD_CLI_OPEN_LISTS_TYPE_BASED_OPEN_LIST_FEATURE_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::open_lists {

void add_type_based_open_list_features(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif