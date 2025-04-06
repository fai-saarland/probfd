#include "probfd/merge_and_shrink/merge_scoring_function_goal_relevance.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/transition_system.h"
#include "probfd/merge_and_shrink/utils.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

vector<double> MergeScoringFunctionGoalRelevance::compute_scores(
    const FactoredTransitionSystem& fts,
    const vector<pair<int, int>>& merge_candidates)
{
    const int num_ts = fts.get_size();
    vector goal_relevant(num_ts, false);

    for (const int ts_index : fts) {
        if (const TransitionSystem& ts = fts.get_transition_system(ts_index);
            is_goal_relevant(ts)) {
            goal_relevant[ts_index] = true;
        }
    }

    vector<double> scores;
    scores.reserve(merge_candidates.size());

    for (auto [ts_index1, ts_index2] : merge_candidates) {
        scores.push_back(
            goal_relevant[ts_index1] || goal_relevant[ts_index2]
                ? 0.0
                : std::numeric_limits<double>::infinity());
    }

    return scores;
}

string MergeScoringFunctionGoalRelevance::name() const
{
    return "goal relevance";
}

} // namespace probfd::merge_and_shrink