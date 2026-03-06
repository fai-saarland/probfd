#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_RPG_EXHAUST_FEATURE_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_RPG_EXHAUST_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::landmarks {

language::parser::InternalFunctionDefinitionBase&
add_landmark_factory_rpg_exhaust_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif