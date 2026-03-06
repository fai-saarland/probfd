#ifndef DOWNWARD_EVALUATORS_G_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_G_EVALUATOR_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::evaluators {

language::parser::InternalFunctionDefinitionBase&
add_g_evaluator_feature(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace downward::cli::evaluators

#endif