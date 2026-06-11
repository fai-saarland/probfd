#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_ZHU_GIVAN_FEATURE_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_ZHU_GIVAN_FEATURE_H

namespace language::plugins {
class RawRegistry;
}

namespace downward::cli::landmarks {

void add_landmark_factory_zhu_givan_feature(
    language::plugins::RawRegistry& raw_registry);

}

#endif