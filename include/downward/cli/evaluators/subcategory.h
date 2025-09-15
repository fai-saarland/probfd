#ifndef DOWNWARD_EVALUATORS_SUBCATEGORY_H
#define DOWNWARD_EVALUATORS_SUBCATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::evaluators {

void add_evaluator_subcategory(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif