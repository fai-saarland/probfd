#ifndef DOWNWARD_EVALUATORS_PREF_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_PREF_EVALUATOR_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::evaluators {

void add_pref_evaluator_feature(
    downward::cli::plugins::Registry& raw_registry);

}

#endif