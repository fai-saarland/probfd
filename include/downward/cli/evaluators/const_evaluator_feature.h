#ifndef DOWNWARD_EVALUATORS_CONST_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_CONST_EVALUATOR_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::evaluators {

void add_const_evaluator_feature(
    language::plugins::Registry& registry);

}

#endif