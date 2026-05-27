#ifndef DOWNWARD_EVALUATORS_G_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_G_EVALUATOR_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::evaluators {

void add_g_evaluator_feature(
    downward::cli::plugins::Registry& registry);

}

#endif