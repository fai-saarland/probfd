#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_FACTORY_TOTAL_ORDER_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_FACTORY_TOTAL_ORDER_H

#include "probfd/merge_and_shrink/merge_scoring_function_factory.h"

#include "probfd/merge_and_shrink/atomic_ts_order.h"
#include "probfd/merge_and_shrink/product_ts_order.h"

#include <memory>

namespace probfd::merge_and_shrink {

class MergeScoringFunctionFactoryTotalOrder
    : public MergeScoringFunctionFactory {
    AtomicTSOrder atomic_ts_order;
    ProductTSOrder product_ts_order;
    bool atomic_before_product;
    int random_seed;

public:
    MergeScoringFunctionFactoryTotalOrder(
        AtomicTSOrder atomic_ts_order,
        ProductTSOrder product_ts_order,
        bool atomic_before_product,
        int random_seed);

    std::unique_ptr<MergeScoringFunction>
    compute_scoring_function(const ProbabilisticTaskTuple& task) override;

private:
    std::string name() const override;

    void dump_function_specific_options(
        downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
