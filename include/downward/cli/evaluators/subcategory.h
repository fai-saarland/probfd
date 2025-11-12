#ifndef DOWNWARD_EVALUATORS_SUBCATEGORY_H
#define DOWNWARD_EVALUATORS_SUBCATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::evaluators {

void add_evaluator_subcategory(
    downward::cli::plugins::Registry& raw_registry);

}

#endif