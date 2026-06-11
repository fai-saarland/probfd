#ifndef DOWNWARD_EVALUATORS_PREF_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_PREF_EVALUATOR_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::evaluators {

void add_pref_evaluator_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif