#ifndef DOWNWARD_EVALUATORS_PREF_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_PREF_EVALUATOR_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::evaluators {

void add_pref_evaluator_feature(
    language::plugins::Registry& registry);

}

#endif