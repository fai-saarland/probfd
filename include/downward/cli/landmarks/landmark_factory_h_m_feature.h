#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_HM_FEATURE_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_HM_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::landmarks {

void add_landmark_factory_hm_feature(
    downward::cli::plugins::Registry& raw_registry);

}

#endif