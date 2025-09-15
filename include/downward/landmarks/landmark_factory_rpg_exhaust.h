#ifndef LANDMARKS_LANDMARK_FACTORY_RPG_EXHAUST_H
#define LANDMARKS_LANDMARK_FACTORY_RPG_EXHAUST_H

#include "downward/landmarks/landmark_factory_relaxation.h"

namespace downward::landmarks {
class LandmarkFactoryRpgExhaust : public LandmarkFactoryRelaxation {
    const bool use_unary_relaxation;

    void generate_relaxed_landmarks(
        const SharedAbstractTask& task,
        Exploration& exploration) override;

public:
    explicit LandmarkFactoryRpgExhaust(
        bool use_unary_relaxation,
        utils::Verbosity verbosity);

    bool supports_conditional_effects() const override;
};
} // namespace landmarks

#endif
