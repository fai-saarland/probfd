#include "probfd/merge_and_shrink/merge_scoring_function_factory_total_order.h"

#include "probfd/merge_and_shrink/merge_scoring_function_total_order.h"

#include "downward/variable_space.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/probabilistic_task.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include <cassert>

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeScoringFunctionFactoryTotalOrder::MergeScoringFunctionFactoryTotalOrder(
    AtomicTSOrder atomic_ts_order,
    ProductTSOrder product_ts_order,
    bool atomic_before_product,
    int random_seed)
    : atomic_ts_order(atomic_ts_order)
    , product_ts_order(product_ts_order)
    , atomic_before_product(atomic_before_product)
    , random_seed(random_seed)
{
}

std::unique_ptr<MergeScoringFunction>
MergeScoringFunctionFactoryTotalOrder::compute_scoring_function(
    const ProbabilisticTaskTuple& task)
{
    return std::make_unique<MergeScoringFunctionTotalOrder>(
        task,
        atomic_ts_order,
        product_ts_order,
        atomic_before_product,
        random_seed);
}

string MergeScoringFunctionFactoryTotalOrder::name() const
{
    return "total order";
}

void MergeScoringFunctionFactoryTotalOrder::dump_function_specific_options(
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
        log.println("Random seed: {}", random_seed);
    }
}

} // namespace probfd::merge_and_shrink
