#ifndef DOWNWARD_EVALUATORS_G_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_G_EVALUATOR_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::evaluators {

language::plugins::InternalFunctionDefinitionBase&
add_g_evaluator_feature(language::plugins::Namespace& nspace);

} // namespace downward::cli::evaluators

#endif