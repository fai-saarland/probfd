#ifndef DOWNWARD_EVALUATORS_CONST_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_CONST_EVALUATOR_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::evaluators {

void add_const_evaluator_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif