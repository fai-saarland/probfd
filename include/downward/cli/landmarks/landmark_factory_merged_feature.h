#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_MERGED_FEATURE_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_MERGED_FEATURE_H

namespace language::plugins {
class InternalFunctionDefinitionBase;
class Namespace;
} // namespace language::plugins

namespace downward::cli::landmarks {

language::plugins::InternalFunctionDefinitionBase&
add_landmark_factory_merged_feature(language::plugins::Namespace& nspace);

}

#endif