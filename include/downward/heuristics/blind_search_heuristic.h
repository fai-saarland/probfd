#ifndef HEURISTICS_BLIND_SEARCH_HEURISTIC_H
#define HEURISTICS_BLIND_SEARCH_HEURISTIC_H

#include "downward/heuristic.h"

namespace downward::blind_search_heuristic {
class BlindSearchHeuristic : public Heuristic {
    int min_operator_cost;

protected:
    virtual int compute_heuristic(const State& ancestor_state) override;

public:
    BlindSearchHeuristic(
        std::shared_ptr<AbstractTask> original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);

    BlindSearchHeuristic(
        std::shared_ptr<AbstractTask> original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& description,
        utils::Verbosity verbosity);
};
} // namespace blind_search_heuristic

#endif
