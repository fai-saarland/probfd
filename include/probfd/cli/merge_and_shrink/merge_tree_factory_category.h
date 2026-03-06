#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_CATEGORY_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::merge_and_shrink {

void add_merge_tree_factory_category(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif