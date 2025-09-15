#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_MERGED_FEATURE_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_MERGED_FEATURE_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::landmarks {

void add_landmark_factory_merged_feature(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif