#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::merge_and_shrink {

language::parser::InternalFunctionDefinitionBase&
add_merge_tree_factory_linear_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif