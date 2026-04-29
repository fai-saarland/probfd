#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_TOTAL_ORDER_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SCORING_FUNCTION_TOTAL_ORDER_FACTORY_H

#include "probfd/merge_and_shrink/merge_scoring_function_factory.h"
#include "probfd/merge_and_shrink/types.h"

#include <memory>

namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd::merge_and_shrink {

class MergeScoringFunctionTotalOrderFactory
    : public MergeScoringFunctionFactory {
    AtomicTSOrder atomic_ts_order;
    ProductTSOrder product_ts_order;
    bool atomic_before_product;
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng;

public:
    MergeScoringFunctionTotalOrderFactory(
        AtomicTSOrder atomic_ts_order,
        ProductTSOrder product_ts_order,
        bool atomic_before_product,
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng);

    std::unique_ptr<MergeScoringFunction>
    create(SharedProbabilisticTask task) override;

private:
    std::string name() const override;

    void dump_function_specific_options(
        downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
