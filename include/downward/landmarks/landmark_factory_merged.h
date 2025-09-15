#ifndef LANDMARKS_LANDMARK_FACTORY_MERGED_H
#define LANDMARKS_LANDMARK_FACTORY_MERGED_H

#include "downward/landmarks/landmark_factory.h"

#include <vector>

namespace downward::landmarks {
class LandmarkFactoryMerged : public LandmarkFactory {
    std::vector<std::shared_ptr<LandmarkFactory>> lm_factories;

    void generate_landmarks(const SharedAbstractTask& task) override;
    void postprocess();
    LandmarkNode* get_matching_landmark(const Landmark& landmark) const;

public:
    LandmarkFactoryMerged(
        const std::vector<std::shared_ptr<LandmarkFactory>>& lm_factories,
        utils::Verbosity verbosity);

    bool supports_conditional_effects() const override;
};
} // namespace downward::landmarks

#endif
