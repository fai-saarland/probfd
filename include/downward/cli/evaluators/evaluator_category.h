#ifndef DOWNWARD_CLI_EVALUATORS_EVALUATOR_CATEGORY_H
#define DOWNWARD_CLI_EVALUATORS_EVALUATOR_CATEGORY_H

namespace language::plugins {
class Namespace;
}

namespace downward::cli::evaluators {

void add_evaluator_category(language::plugins::Namespace& nspace);

}

#endif