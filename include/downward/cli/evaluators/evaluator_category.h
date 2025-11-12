#ifndef DOWNWARD_CLI_EVALUATORS_EVALUATOR_CATEGORY_H
#define DOWNWARD_CLI_EVALUATORS_EVALUATOR_CATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::evaluators {

void add_evaluator_category(
    downward::cli::plugins::Registry& raw_registry);

}

#endif