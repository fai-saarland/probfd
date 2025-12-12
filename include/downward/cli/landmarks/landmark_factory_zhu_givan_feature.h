#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_ZHU_GIVAN_FEATURE_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_ZHU_GIVAN_FEATURE_H

namespace language::plugins {
class Registry;
}

namespace downward::cli::landmarks {

void add_landmark_factory_zhu_givan_feature(
    language::plugins::Registry& registry);

}

#endif