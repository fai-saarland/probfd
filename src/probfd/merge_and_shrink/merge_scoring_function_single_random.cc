#include "probfd/merge_and_shrink/merge_scoring_function_single_random.h"

#include "probfd/merge_and_shrink/types.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeScoringFunctionSingleRandom::MergeScoringFunctionSingleRandom(
    int random_seed)
    : random_seed(random_seed)
    , rng(utils::get_rng(random_seed))
{
}

vector<double> MergeScoringFunctionSingleRandom::compute_scores(
    const FactoredTransitionSystem&,
    const vector<pair<int, int>>& merge_candidates)
{
    const int chosen_index = rng->random(merge_candidates.size());

    vector<double> scores;
    scores.reserve(merge_candidates.size());

    for (size_t candidate_index = 0; candidate_index < merge_candidates.size();
         ++candidate_index) {
        scores.push_back(
            static_cast<int>(candidate_index) == chosen_index
                ? 0.0
                : std::numeric_limits<double>::infinity());
    }

    return scores;
}

string MergeScoringFunctionSingleRandom::name() const
{
    return "single random";
}

void MergeScoringFunctionSingleRandom::dump_function_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log << "Random seed: " << random_seed << endl;
    }
}

} // namespace probfd::merge_and_shrink