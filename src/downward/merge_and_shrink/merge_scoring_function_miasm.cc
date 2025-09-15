#include "downward/merge_and_shrink/merge_scoring_function_miasm.h"

#include "downward/merge_and_shrink/distances.h"
#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_scoring_function_miasm_utils.h"
#include "downward/merge_and_shrink/shrink_strategy.h"
#include "downward/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"

#include "downward/abstract_task.h"
#include "downward/state.h"

using namespace std;

namespace downward::merge_and_shrink {
MergeScoringFunctionMIASM::MergeScoringFunctionMIASM(
    shared_ptr<ShrinkStrategy> shrink_strategy,
    int max_states,
    int max_states_before_merge,
    int threshold_before_merge,
    bool use_caching)
    : use_caching(use_caching)
    , shrink_strategy(move(shrink_strategy))
    , max_states(max_states)
    , max_states_before_merge(max_states_before_merge)
    , shrink_threshold_before_merge(threshold_before_merge)
    , silent_log(utils::get_silent_log())
{
}

vector<double> MergeScoringFunctionMIASM::compute_scores(
    const FactoredTransitionSystem& fts,
    const vector<pair<int, int>>& merge_candidates)
{
    vector<double> scores;
    scores.reserve(merge_candidates.size());
    for (pair<int, int> merge_candidate : merge_candidates) {
        double score;
        int index1 = merge_candidate.first;
        int index2 = merge_candidate.second;
        if (use_caching &&
            cached_scores_by_merge_candidate_indices[index1][index2]) {
            score = *cached_scores_by_merge_candidate_indices[index1][index2];
        } else {
            unique_ptr<TransitionSystem> product =
                shrink_before_merge_externally(
                    fts,
                    index1,
                    index2,
                    *shrink_strategy,
                    max_states,
                    max_states_before_merge,
                    shrink_threshold_before_merge,
                    silent_log);

            // Compute distances for the product and count the alive states.
            unique_ptr<Distances> distances =
                std::make_unique<Distances>(*product);
            const bool compute_init_distances = true;
            const bool compute_goal_distances = true;
            distances->compute_distances(
                compute_init_distances,
                compute_goal_distances,
                silent_log);
            int num_states = product->get_size();
            int alive_states_count = 0;
            for (int state = 0; state < num_states; ++state) {
                if (distances->get_init_distance(state) != INF &&
                    distances->get_goal_distance(state) != INF) {
                    ++alive_states_count;
                }
            }

            /*
              Compute the score as the ratio of alive states of the product
              compared to the number of states of the full product.
            */
            assert(num_states);
            score = static_cast<double>(alive_states_count) /
                    static_cast<double>(num_states);
            if (use_caching) {
                cached_scores_by_merge_candidate_indices[index1][index2] =
                    score;
            }
        }
        scores.push_back(score);
    }
    return scores;
}

void MergeScoringFunctionMIASM::initialize(const AbstractTaskTuple& task)
{
    const auto& variables = get_variables(task);

    initialized = true;
    int num_variables = variables.size();
    int max_factor_index = 2 * num_variables - 1;
    cached_scores_by_merge_candidate_indices.resize(
        max_factor_index,
        vector<optional<double>>(max_factor_index));
}

void MergeScoringFunctionMIASM::dump_function_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log << "Use caching: " << (use_caching ? "yes" : "no") << endl;
    }
}

string MergeScoringFunctionMIASM::name() const
{
    return "miasm";
}

} // namespace downward::merge_and_shrink
