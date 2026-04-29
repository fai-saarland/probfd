#include "probfd/merge_and_shrink/merge_scoring_function_total_order_factory.h"

#include "probfd/merge_and_shrink/merge_scoring_function_total_order.h"

#include "downward/utils/logging.h"

using namespace downward;

namespace probfd::merge_and_shrink {

MergeScoringFunctionTotalOrderFactory::MergeScoringFunctionTotalOrderFactory(
    AtomicTSOrder atomic_ts_order,
    ProductTSOrder product_ts_order,
    bool atomic_before_product,
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : atomic_ts_order(atomic_ts_order)
    , product_ts_order(product_ts_order)
    , atomic_before_product(atomic_before_product)
    , rng(std::move(rng))
{
}

std::unique_ptr<MergeScoringFunction>
MergeScoringFunctionTotalOrderFactory::create(SharedProbabilisticTask task)
{
    return std::make_unique<MergeScoringFunctionTotalOrder>(
        to_refs(task),
        atomic_ts_order,
        product_ts_order,
        atomic_before_product,
        rng);
}

std::string MergeScoringFunctionTotalOrderFactory::name() const
{ return "total order"; }

void MergeScoringFunctionTotalOrderFactory::dump_function_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log.print("Atomic transition system order: ");
        switch (atomic_ts_order) {
        case AtomicTSOrder::REVERSE_LEVEL: log.print("reverse level"); break;
        case AtomicTSOrder::LEVEL: log.print("level"); break;
        case AtomicTSOrder::RANDOM: log.print("random"); break;
        }
        log.println();

        log.print("Product transition system order: ");
        switch (product_ts_order) {
        case ProductTSOrder::OLD_TO_NEW: log.print("old to new"); break;
        case ProductTSOrder::NEW_TO_OLD: log.print("new to old"); break;
        case ProductTSOrder::RANDOM: log.print("random"); break;
        }
        log.println();

        log.println(
            "Consider {} transition systems",
            atomic_before_product ? "atomic before product"
                                  : "product before atomic");
    }
}

} // namespace probfd::merge_and_shrink
