#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_HM_FEATURE_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_HM_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
}

namespace downward::cli::landmarks {

language::plugins::InternalFunctionDefinitionBase&
add_landmark_factory_hm_feature(language::plugins::Namespace& nspace);

}

#endif