#include "probfd/merge_and_shrink/shrink_strategy_bucket_based.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"
#include "downward/views/transform.h"

#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

ShrinkStrategyBucketBased::ShrinkStrategyBucketBased(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng(std::move(rng))
{
}

StateEquivalenceRelation ShrinkStrategyBucketBased::compute_abstraction(
    const vector<Bucket>& buckets,
    int target_size,
    utils::LogProxy& log) const
{
    bool show_combine_buckets_warning = true;
    StateEquivalenceRelation equiv_relation;
    equiv_relation.reserve(target_size);

    constexpr auto get_size = [](const auto& x) { return x.size(); };

    size_t num_states_to_go = std::ranges::fold_left(
        buckets | downward::views::transform<get_size>,
        0U,
        std::plus{});

    for (size_t bucket_no = 0; bucket_no < buckets.size(); ++bucket_no) {
        const vector<int>& bucket = buckets[bucket_no];
        const int states_used_up = static_cast<int>(equiv_relation.size());
        const int remaining_state_budget = target_size - states_used_up;
        num_states_to_go -= bucket.size();

        if (const int budget_for_this_bucket =
                remaining_state_budget - num_states_to_go;
            std::cmp_greater_equal(budget_for_this_bucket, bucket.size())) {
            // Each state in bucket can become a singleton group.
            for (const int i : bucket) {
                equiv_relation.emplace_back(StateEquivalenceClass{i});
            }
        } else if (budget_for_this_bucket <= 1) {
            // The whole bucket must form one group.
            if (const int remaining_buckets = buckets.size() - bucket_no;
                remaining_state_budget >= remaining_buckets) {
                equiv_relation.emplace_back();
            } else {
                if (bucket_no == 0) equiv_relation.emplace_back();
                if (show_combine_buckets_warning) {
                    show_combine_buckets_warning = false;
                    log.println("Very small node limit, must combine buckets.");
                }
            }
            StateEquivalenceClass& group = equiv_relation.back();
            group.insert_range_after(group.before_begin(), bucket);
        } else {
            // Complicated case: must combine until bucket budget is met.
            // First create singleton groups.
            vector<StateEquivalenceClass> groups(bucket.size());
            for (size_t i = 0; i < bucket.size(); ++i)
                groups[i].push_front(bucket[i]);

            // Then combine groups until required size is reached.
            assert(
                budget_for_this_bucket >= 2 &&
                std::cmp_less(budget_for_this_bucket, groups.size()));
            while (std::cmp_greater(groups.size(), budget_for_this_bucket)) {
                auto it1 = rng->choose(groups);
                auto it2 = it1;
                while (it1 == it2) {
                    it2 = rng->choose(groups);
                }
                it1->splice_after(it1->before_begin(), *it2);
                swap(*it2, groups.back());
                assert(groups.back().empty());
                groups.pop_back();
            }

            // Finally add these groups to the result.
            for (auto& group : groups) {
                equiv_relation.emplace_back().swap(group);
            }
        }
    }

    return equiv_relation;
}

StateEquivalenceRelation
ShrinkStrategyBucketBased::compute_equivalence_relation(
    const Labels&,
    const TransitionSystem& ts,
    const Distances& distances,
    int target_size,
    utils::LogProxy& log) const
{
    const vector<Bucket> buckets = partition_into_buckets(ts, distances);
    return compute_abstraction(buckets, target_size, log);
}

} // namespace probfd::merge_and_shrink