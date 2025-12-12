#ifndef DOWNWARD_CLI_EVALUATORS_EVALUATOR_CATEGORY_H
#define DOWNWARD_CLI_EVALUATORS_EVALUATOR_CATEGORY_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::evaluators {

void add_evaluator_category(
    language::plugins::Registry& registry);

}

#endif