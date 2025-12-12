#ifndef DOWNWARD_EVALUATORS_WEIGHTED_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_WEIGHTED_EVALUATOR_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::evaluators {

void add_weighted_evaluator_feature(
    language::plugins::Registry& registry);

}

#endif