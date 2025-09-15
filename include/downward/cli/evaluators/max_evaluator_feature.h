#ifndef DOWNWARD_EVALUATORS_MAX_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_MAX_EVALUATOR_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::evaluators {

void add_max_evaluator_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif