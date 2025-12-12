#ifndef DOWNWARD_EVALUATORS_SUBCATEGORY_H
#define DOWNWARD_EVALUATORS_SUBCATEGORY_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::evaluators {

void add_evaluator_subcategory(
    language::plugins::Registry& registry);

}

#endif