#include "downward/merge_and_shrink/merge_selector_score_based_filtering.h"

#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_scoring_function.h"

#include <cassert>

using namespace std;

namespace downward::merge_and_shrink {
MergeSelectorScoreBasedFiltering::MergeSelectorScoreBasedFiltering(
    const vector<shared_ptr<MergeScoringFunction>>& scoring_functions)
    : merge_scoring_functions(scoring_functions)
{
}

static vector<pair<int, int>> get_remaining_candidates(
    const vector<pair<int, int>>& merge_candidates,
    const vector<double>& scores)
{
    assert(merge_candidates.size() == scores.size());
    double best_score = INF;
    for (double score : scores) {
        if (score < best_score) {
            best_score = score;
        }
    }

    vector<pair<int, int>> result;
    for (size_t i = 0; i < scores.size(); ++i) {
        if (scores[i] == best_score) {
            result.push_back(merge_candidates[i]);
        }
    }
    return result;
}

pair<int, int> MergeSelectorScoreBasedFiltering::select_merge(
    const FactoredTransitionSystem& fts,
    const vector<int>& indices_subset) const
{
    vector<pair<int, int>> merge_candidates =
        compute_merge_candidates(fts, indices_subset);

    for (const shared_ptr<MergeScoringFunction>& scoring_function :
         merge_scoring_functions) {
        vector<double> scores =
            scoring_function->compute_scores(fts, merge_candidates);
        merge_candidates = get_remaining_candidates(merge_candidates, scores);
        if (merge_candidates.size() == 1) {
            break;
        }
    }

    if (merge_candidates.size() > 1) {
        cerr << "More than one merge candidate remained after computing all "
                "scores! Did you forget to include a uniquely tie-breaking "
                "scoring function, e.g. total_order or single_random?"
             << endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

    return merge_candidates.front();
}

void MergeSelectorScoreBasedFiltering::initialize(const TaskProxy& task_proxy)
{
    for (shared_ptr<MergeScoringFunction>& scoring_function :
         merge_scoring_functions) {
        scoring_function->initialize(task_proxy);
    }
}

string MergeSelectorScoreBasedFiltering::name() const
{
    return "score based filtering";
}

void MergeSelectorScoreBasedFiltering::dump_selector_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        for (const shared_ptr<MergeScoringFunction>& scoring_function :
             merge_scoring_functions) {
            scoring_function->dump_options(log);
        }
    }
}

bool MergeSelectorScoreBasedFiltering::requires_init_distances() const
{
    for (const shared_ptr<MergeScoringFunction>& scoring_function :
         merge_scoring_functions) {
        if (scoring_function->requires_init_distances()) {
            return true;
        }
    }
    return false;
}

bool MergeSelectorScoreBasedFiltering::requires_goal_distances() const
{
    for (const shared_ptr<MergeScoringFunction>& scoring_function :
         merge_scoring_functions) {
        if (scoring_function->requires_goal_distances()) {
            return true;
        }
    }
    return false;
}

} // namespace merge_and_shrink
