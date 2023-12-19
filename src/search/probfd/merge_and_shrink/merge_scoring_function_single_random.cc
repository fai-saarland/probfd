#include "probfd/merge_and_shrink/merge_scoring_function_single_random.h"

#include "probfd/merge_and_shrink/types.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

using namespace std;

namespace probfd::merge_and_shrink {

MergeScoringFunctionSingleRandom::MergeScoringFunctionSingleRandom(
    const plugins::Options& options)
    : random_seed(options.get<int>("random_seed"))
    , rng(utils::parse_rng_from_options(options))
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

class MergeScoringFunctionSingleRandomFeature
    : public plugins::
          TypedFeature<MergeScoringFunction, MergeScoringFunctionSingleRandom> {
public:
    MergeScoringFunctionSingleRandomFeature()
        : TypedFeature("psingle_random")
    {
        document_title("Single random");
        document_synopsis("This scoring function assigns exactly one merge "
                          "candidate a score of "
                          "0, chosen randomly, and infinity to all others.");

        utils::add_rng_options(*this);
    }
};

static plugins::FeaturePlugin<MergeScoringFunctionSingleRandomFeature> _plugin;

} // namespace probfd::merge_and_shrink