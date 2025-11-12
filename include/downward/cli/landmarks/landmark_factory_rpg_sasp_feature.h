#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_RPG_SASP_FEATURE_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_RPG_SASP_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::landmarks {

void add_landmark_factory_rpg_sasp_feature(
    downward::cli::plugins::Registry& raw_registry);

}

#endif