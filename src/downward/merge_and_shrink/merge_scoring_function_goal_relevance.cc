#include "downward/merge_and_shrink/merge_scoring_function_goal_relevance.h"

#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/transition_system.h"
#include "downward/merge_and_shrink/utils.h"

using namespace std;

namespace merge_and_shrink {
vector<double> MergeScoringFunctionGoalRelevance::compute_scores(
    const FactoredTransitionSystem& fts,
    const vector<pair<int, int>>& merge_candidates)
{
    int num_ts = fts.get_size();
    vector<bool> goal_relevant(num_ts, false);
    for (int ts_index : fts) {
        const TransitionSystem& ts = fts.get_transition_system(ts_index);
        if (is_goal_relevant(ts)) {
            goal_relevant[ts_index] = true;
        }
    }

    vector<double> scores;
    scores.reserve(merge_candidates.size());
    for (pair<int, int> merge_candidate : merge_candidates) {
        int ts_index1 = merge_candidate.first;
        int ts_index2 = merge_candidate.second;
        int score = INF;
        if (goal_relevant[ts_index1] || goal_relevant[ts_index2]) {
            score = 0;
        }
        scores.push_back(score);
    }
    return scores;
}

string MergeScoringFunctionGoalRelevance::name() const
{
    return "goal relevance";
}

} // namespace merge_and_shrink
