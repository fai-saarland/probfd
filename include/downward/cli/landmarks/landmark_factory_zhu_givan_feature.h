#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_ZHU_GIVAN_FEATURE_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_ZHU_GIVAN_FEATURE_H

namespace language::parser {
class InternalFunctionDefinitionBase;
class NamespaceLevelDeclarationList;
} // namespace language::parser

namespace downward::cli::landmarks {

language::parser::InternalFunctionDefinitionBase&
add_landmark_factory_zhu_givan_feature(language::parser::NamespaceLevelDeclarationList& nspace);

}

#endif