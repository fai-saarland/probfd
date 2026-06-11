#ifndef DOWNWARD_EVALUATORS_SUBCATEGORY_H
#define DOWNWARD_EVALUATORS_SUBCATEGORY_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::evaluators {

void add_evaluator_subcategory(
    language::plugins::RawRegistry& raw_registry);

}

#endif