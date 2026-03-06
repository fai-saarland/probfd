#ifndef DOWNWARD_CLI_OPERATOR_COST_CATEGORY_H
#define DOWNWARD_CLI_OPERATOR_COST_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli {

void add_operator_cost_category(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif