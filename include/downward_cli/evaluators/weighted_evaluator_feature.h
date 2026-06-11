#ifndef DOWNWARD_EVALUATORS_WEIGHTED_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_WEIGHTED_EVALUATOR_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::evaluators {

void add_weighted_evaluator_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif