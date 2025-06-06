#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_TOTAL_ORDER_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_TOTAL_ORDER_H

#include "probfd/merge_and_shrink/merge_scoring_function.h"

#include <memory>
#include <tuple>

namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd::merge_and_shrink {

enum class AtomicTSOrder { REVERSE_LEVEL, LEVEL, RANDOM };
enum class ProductTSOrder { OLD_TO_NEW, NEW_TO_OLD, RANDOM };

class MergeScoringFunctionTotalOrder : public MergeScoringFunction {
    AtomicTSOrder atomic_ts_order;
    ProductTSOrder product_ts_order;
    bool atomic_before_product;
    int random_seed; // only for dump options
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng;
    std::vector<std::pair<int, int>> merge_candidate_order;

public:
    MergeScoringFunctionTotalOrder(
        AtomicTSOrder atomic_ts_order,
        ProductTSOrder product_ts_order,
        bool atomic_before_product,
        int random_seed);

    std::vector<double> compute_scores(
        const FactoredTransitionSystem& fts,
        const std::vector<std::pair<int, int>>& merge_candidates) override;
    void initialize(const ProbabilisticTaskProxy& task_proxy) override;

    bool requires_liveness() const override { return false; }

    bool requires_goal_distances() const override { return false; }

private:
    std::string name() const override;
    void dump_function_specific_options(
        downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
