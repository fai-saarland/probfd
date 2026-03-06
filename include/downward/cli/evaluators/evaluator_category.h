#ifndef DOWNWARD_CLI_EVALUATORS_EVALUATOR_CATEGORY_H
#define DOWNWARD_CLI_EVALUATORS_EVALUATOR_CATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace downward::cli::evaluators {

void add_evaluator_category(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif