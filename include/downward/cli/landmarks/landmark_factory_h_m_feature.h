#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_HM_FEATURE_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_HM_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
}

namespace downward::cli::landmarks {

language::parser::InternalFunctionDefinitionBase&
add_landmark_factory_hm_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif