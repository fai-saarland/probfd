#ifndef DOWNWARD_EVALUATORS_WEIGHTED_EVALUATOR_FEATURE_H
#define DOWNWARD_EVALUATORS_WEIGHTED_EVALUATOR_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::evaluators {

language::parser::InternalFunctionDefinitionBase&
add_weighted_evaluator_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif