#ifndef DOWNWARD_CLI_EVALUATORS_EVALUATOR_CATEGORY_H
#define DOWNWARD_CLI_EVALUATORS_EVALUATOR_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::evaluators {

void add_evaluator_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif