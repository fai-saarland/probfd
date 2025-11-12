#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_RPG_EXHAUST_FEATURE_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_RPG_EXHAUST_FEATURE_H

namespace downward::cli::plugins {
class Registry;
}

namespace downward::cli::landmarks {

void add_landmark_factory_rpg_exhaust_feature(
    downward::cli::plugins::Registry& raw_registry);

}

#endif