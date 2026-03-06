#ifndef DOWNWARD_CLI_PRUNING_PRUNING_METHOD_CATEGORY_H
#define DOWNWARD_CLI_PRUNING_PRUNING_METHOD_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::pruning {

void add_pruning_method_category(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif