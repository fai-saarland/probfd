#ifndef HEURISTICS_GOAL_COUNT_HEURISTIC_H
#define HEURISTICS_GOAL_COUNT_HEURISTIC_H

#include "downward/heuristic.h"

namespace downward::goal_count_heuristic {
class GoalCountHeuristic : public Heuristic {
protected:
    virtual int compute_heuristic(const State& ancestor_state) override;

public:
    GoalCountHeuristic(
        std::shared_ptr<AbstractTask> original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    GoalCountHeuristic(
        std::shared_ptr<AbstractTask> original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);
};
} // namespace goal_count_heuristic

#endif
