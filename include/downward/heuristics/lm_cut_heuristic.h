#ifndef HEURISTICS_LM_CUT_HEURISTIC_H
#define HEURISTICS_LM_CUT_HEURISTIC_H

#include "downward/heuristic.h"

#include <memory>

namespace downward::lm_cut_heuristic {
class LandmarkCutLandmarks;

class LandmarkCutHeuristic : public Heuristic {
    std::unique_ptr<LandmarkCutLandmarks> landmark_generator;

    int compute_heuristic(const State& ancestor_state) override;

public:
    LandmarkCutHeuristic(
        SharedAbstractTask original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    LandmarkCutHeuristic(
        SharedAbstractTask original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    ~LandmarkCutHeuristic() override;
};
} // namespace lm_cut_heuristic

#endif
