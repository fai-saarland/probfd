#include "downward/merge_and_shrink/merge_scoring_function_dfp.h"

#include "downward/merge_and_shrink/distances.h"
#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/labels.h"
#include "downward/merge_and_shrink/transition_system.h"

#include <cassert>

using namespace std;

namespace downward::merge_and_shrink {
static vector<int>
compute_label_ranks(const FactoredTransitionSystem& fts, int index)
{
    const TransitionSystem& ts = fts.get_transition_system(index);
    const Distances& distances = fts.get_distances(index);
    assert(distances.are_goal_distances_computed());
    int num_labels = fts.get_labels().get_num_total_labels();
    // Irrelevant (and inactive, i.e. reduced) labels have a dummy rank of -1
    vector<int> label_ranks(num_labels, -1);

    for (const LocalLabelInfo& local_label_info : ts) {
        const LabelGroup& label_group = local_label_info.get_label_group();
        const vector<Transition>& transitions =
            local_label_info.get_transitions();
        // Relevant labels with no transitions have a rank of infinity.
        int label_rank = INF;
        bool group_relevant = false;
        if (static_cast<int>(transitions.size()) == ts.get_size()) {
            /*
              A label group is irrelevant in the earlier notion if it has
              exactly a self loop transition for every state.
            */
            for (const Transition& transition : transitions) {
                if (transition.target != transition.src) {
                    group_relevant = true;
                    break;
                }
            }
        } else {
            group_relevant = true;
        }
        if (!group_relevant) {
            label_rank = -1;
        } else {
            for (const Transition& transition : transitions) {
                label_rank =
                    min(label_rank,
                        distances.get_goal_distance(transition.target));
            }
        }
        for (int label : label_group) {
            label_ranks[label] = label_rank;
        }
    }

    return label_ranks;
}

vector<double> MergeScoringFunctionDFP::compute_scores(
    const FactoredTransitionSystem& fts,
    const vector<pair<int, int>>& merge_candidates)
{
    int num_ts = fts.get_size();

    vector<vector<int>> transition_system_label_ranks(num_ts);
    vector<double> scores;
    scores.reserve(merge_candidates.size());

    // Go over all pairs of transition systems and compute their weight.
    for (pair<int, int> merge_candidate : merge_candidates) {
        int ts_index1 = merge_candidate.first;
        int ts_index2 = merge_candidate.second;

        vector<int>& label_ranks1 = transition_system_label_ranks[ts_index1];
        if (label_ranks1.empty()) {
            label_ranks1 = compute_label_ranks(fts, ts_index1);
        }
        vector<int>& label_ranks2 = transition_system_label_ranks[ts_index2];
        if (label_ranks2.empty()) {
            label_ranks2 = compute_label_ranks(fts, ts_index2);
        }
        assert(label_ranks1.size() == label_ranks2.size());

        // Compute the weight associated with this pair
        int pair_weight = INF;
        for (size_t i = 0; i < label_ranks1.size(); ++i) {
            if (label_ranks1[i] != -1 && label_ranks2[i] != -1) {
                // label is relevant in both transition_systems
                int max_label_rank = max(label_ranks1[i], label_ranks2[i]);
                pair_weight = min(pair_weight, max_label_rank);
            }
        }
        scores.push_back(pair_weight);
    }
    return scores;
}

string MergeScoringFunctionDFP::name() const
{
    return "dfp";
}

} // namespace merge_and_shrink
