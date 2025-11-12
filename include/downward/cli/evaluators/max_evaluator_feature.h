#ifndef DOWNWARD_EVALUATORS_MAX_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_MAX_EVALUATOR_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::evaluators {

void add_max_evaluator_feature(
    downward::cli::plugins::Registry& raw_registry);

}

#endif