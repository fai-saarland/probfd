#ifndef HEURISTICS_LM_CUT_HEURISTIC_H
#define HEURISTICS_LM_CUT_HEURISTIC_H

#include "downward/heuristic.h"

#include <memory>

namespace lm_cut_heuristic {
class LandmarkCutLandmarks;

class LandmarkCutHeuristic : public Heuristic {
    std::unique_ptr<LandmarkCutLandmarks> landmark_generator;

    virtual int compute_heuristic(const State& ancestor_state) override;

public:
    LandmarkCutHeuristic(
        const std::shared_ptr<AbstractTask>& transform,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    ~LandmarkCutHeuristic() override;
};
} // namespace lm_cut_heuristic

#endif
