#ifndef DOWNWARD_LANDMARKS_LANDMARK_FACTORY_CATEGORY_H
#define DOWNWARD_LANDMARKS_LANDMARK_FACTORY_CATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace downward::cli::landmarks {

void add_landmark_factory_category(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif