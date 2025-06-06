#ifndef LANDMARKS_LANDMARK_SUM_HEURISTIC_H
#define LANDMARKS_LANDMARK_SUM_HEURISTIC_H

#include "downward/landmarks/landmark_heuristic.h"

namespace downward::landmarks {
class LandmarkSumHeuristic : public LandmarkHeuristic {
    const bool dead_ends_reliable;

    std::vector<int> min_first_achiever_costs;
    std::vector<int> min_possible_achiever_costs;

    int
    get_min_cost_of_achievers(const std::unordered_set<int>& achievers) const;
    void compute_landmark_costs();

    int get_heuristic_value(const State& ancestor_state) override;

public:
    LandmarkSumHeuristic(
        const std::shared_ptr<LandmarkFactory>& lm_factory,
        bool pref,
        bool prog_goal,
        bool prog_gn,
        bool prog_r,
        std::shared_ptr<AbstractTask> original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    LandmarkSumHeuristic(
        const std::shared_ptr<LandmarkFactory>& lm_factory,
        bool pref,
        bool prog_goal,
        bool prog_gn,
        bool prog_r,
        std::shared_ptr<AbstractTask> original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    virtual bool dead_ends_are_reliable() const override;
};
} // namespace landmarks

#endif
