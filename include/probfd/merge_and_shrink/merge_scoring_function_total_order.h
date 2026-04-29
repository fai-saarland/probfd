#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_TOTAL_ORDER_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_TOTAL_ORDER_H

#include "probfd/merge_and_shrink/merge_scoring_function.h"
#include "probfd/merge_and_shrink/types.h"

#include "probfd/probabilistic_task.h"

#include <memory>
#include <tuple>

namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd::merge_and_shrink {

class MergeScoringFunctionTotalOrder : public MergeScoringFunction {
    AtomicTSOrder atomic_ts_order;
    ProductTSOrder product_ts_order;
    bool atomic_before_product;
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng;
    std::vector<std::pair<int, int>> merge_candidate_order;

public:
    MergeScoringFunctionTotalOrder(
        const ProbabilisticTaskTuple& task,
        AtomicTSOrder atomic_ts_order,
        ProductTSOrder product_ts_order,
        bool atomic_before_product,
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng);

    std::vector<double> compute_scores(
        const FactoredTransitionSystem& fts,
        const std::vector<std::pair<int, int>>& merge_candidates) override;

    bool requires_liveness() const override { return false; }

    bool requires_goal_distances() const override { return false; }
};

} // namespace probfd::merge_and_shrink

#endif
