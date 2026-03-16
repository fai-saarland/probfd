#include "downward/merge_and_shrink/merge_scoring_function_single_random.h"

#include "downward/merge_and_shrink/types.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"

#include <cassert>

using namespace std;

namespace downward::merge_and_shrink {
MergeScoringFunctionSingleRandom::MergeScoringFunctionSingleRandom(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng(std::move(rng))
{
}

vector<double> MergeScoringFunctionSingleRandom::compute_scores(
    const FactoredTransitionSystem&,
    const vector<pair<int, int>>& merge_candidates)
{
    int chosen_index = rng->random(merge_candidates.size());
    vector<double> scores;
    scores.reserve(merge_candidates.size());
    for (size_t candidate_index = 0; candidate_index < merge_candidates.size();
         ++candidate_index) {
        if (static_cast<int>(candidate_index) == chosen_index) {
            scores.push_back(0);
        } else {
            scores.push_back(INF);
        }
    }
    return scores;
}

string MergeScoringFunctionSingleRandom::name() const
{
    return "single random";
}

void MergeScoringFunctionSingleRandom::dump_function_specific_options(
    utils::LogProxy&) const
{
}

} // namespace downward::merge_and_shrink
