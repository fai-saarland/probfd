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

MergeScoringFunctionTotalOrder::MergeScoringFunctionTotalOrder(
    AtomicTSOrder atomic_ts_order,
    ProductTSOrder product_ts_order,
    bool atomic_before_product,
    int random_seed)
    : atomic_ts_order(atomic_ts_order)
    , product_ts_order(product_ts_order)
    , atomic_before_product(atomic_before_product)
    , random_seed(random_seed)
    , rng(utils::get_rng(random_seed))
{
}

static std::pair<int, int> rotate(const std::pair<int, int>& p)
{
    return std::make_pair(p.second, p.first);
}

vector<double> MergeScoringFunctionTotalOrder::compute_scores(
    const FactoredTransitionSystem&,
    const vector<pair<int, int>>& merge_candidates)
{
    assert(initialized);

    vector<double> scores;
    scores.reserve(merge_candidates.size());

    for (size_t candidate_index = 0; candidate_index < merge_candidates.size();
         ++candidate_index) {
        auto merge_candidate = merge_candidates[candidate_index];
        for (size_t merge_candidate_order_index = 0;
             merge_candidate_order_index < merge_candidate_order.size();
             ++merge_candidate_order_index) {
            auto other_candidate =
                merge_candidate_order[merge_candidate_order_index];
            if (merge_candidate == other_candidate ||
                merge_candidate == rotate(other_candidate)) {
                // use the index in the merge candidate order as score
                scores.push_back(merge_candidate_order_index);
                break;
            }
        }
        // We must have inserted a score for the current candidate.
        assert(scores.size() == candidate_index + 1);
    }
    return scores;
}

void MergeScoringFunctionTotalOrder::initialize(
    const ProbabilisticTaskTuple& task)
{
    initialized = true;

    const auto& variables = get_variables(task);

    const int num_variables = variables.size();
    const int max_transition_system_count = num_variables * 2 - 1;

    // Compute the order in which atomic transition systems are considered
    vector<int> atomic_tso(std::from_range, std::views::iota(0, num_variables));

    if (atomic_ts_order == AtomicTSOrder::LEVEL) {
        ranges::reverse(atomic_tso);
    } else if (atomic_ts_order == AtomicTSOrder::RANDOM) {
        rng->shuffle(atomic_tso);
    }

    // Compute the order in which product transition systems are considered
    vector<int> product_tso(
        std::from_range,
        std::views::iota(num_variables, max_transition_system_count));

    if (product_ts_order == ProductTSOrder::NEW_TO_OLD) {
        ranges::reverse(product_tso);
    } else if (product_ts_order == ProductTSOrder::RANDOM) {
        rng->shuffle(product_tso);
    }

    vector<int> transition_system_order;
    transition_system_order.reserve(max_transition_system_count);

    // Put the orders in the correct order
    if (atomic_before_product) {
        transition_system_order.append_range(atomic_tso);
        transition_system_order.append_range(product_tso);
    } else {
        transition_system_order.append_range(product_tso);
        transition_system_order.append_range(atomic_tso);
    }

    merge_candidate_order.reserve(
        max_transition_system_count * max_transition_system_count / 2);

    for (size_t i = 0; i < transition_system_order.size(); ++i) {
        for (size_t j = i + 1; j < transition_system_order.size(); ++j) {
            merge_candidate_order.emplace_back(
                transition_system_order[i],
                transition_system_order[j]);
        }
    }
}

string MergeScoringFunctionTotalOrder::name() const
{
    return "total order";
}

void MergeScoringFunctionTotalOrder::dump_function_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log << "Atomic transition system order: ";
        switch (atomic_ts_order) {
        case AtomicTSOrder::REVERSE_LEVEL: log << "reverse level"; break;
        case AtomicTSOrder::LEVEL: log << "level"; break;
        case AtomicTSOrder::RANDOM: log << "random"; break;
        }
        log << endl;

        log << "Product transition system order: ";
        switch (product_ts_order) {
        case ProductTSOrder::OLD_TO_NEW: log << "old to new"; break;
        case ProductTSOrder::NEW_TO_OLD: log << "new to old"; break;
        case ProductTSOrder::RANDOM: log << "random"; break;
        }
        log << endl;

        log << "Consider "
            << (atomic_before_product ? "atomic before product"
                                      : "product before atomic")
            << " transition systems" << endl;
        log << "Random seed: " << random_seed << endl;
    }
}

} // namespace probfd::merge_and_shrink
