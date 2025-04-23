#include "probfd/merge_and_shrink/merge_scoring_function_miasm.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_scoring_function_miasm_utils.h"
#include "probfd/merge_and_shrink/shrink_strategy.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/probabilistic_task.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeScoringFunctionMIASM::MergeScoringFunctionMIASM(
    bool use_caching,
    shared_ptr<ShrinkStrategy> shrink_strategy,
    int max_states,
    int max_states_before_merge,
    int threshold_before_merge)
    : use_caching(use_caching)
    , shrink_strategy(std::move(shrink_strategy))
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

    for (auto [index1, index2] : merge_candidates) {
        if (use_caching) {
            if (auto opt_score =
                    cached_scores_by_merge_candidate_indices[index1][index2]) {
                scores.push_back(*opt_score);
                continue;
            }
        }

        unique_ptr<TransitionSystem> product = shrink_before_merge_externally(
            fts,
            index1,
            index2,
            *shrink_strategy,
            max_states,
            max_states_before_merge,
            shrink_threshold_before_merge,
            silent_log);

        // Compute distances for the product and count the alive states.
        Distances distances;
        distances
            .compute_distances(fts.get_labels(), *product, true, silent_log);

        const int num_states = product->get_size();
        int alive_states_count = 0;

        for (int state = 0; state < num_states; ++state) {
            if (distances.is_alive(state)) { ++alive_states_count; }
        }

        /*
            Compute the score as the ratio of alive states of the product
            compared to the number of states of the full product.
        */
        assert(num_states > 0);
        double score = static_cast<double>(alive_states_count) /
                       static_cast<double>(num_states);

        if (use_caching) {
            cached_scores_by_merge_candidate_indices[index1][index2] = score;
        }

        scores.push_back(score);
    }

    return scores;
}

void MergeScoringFunctionMIASM::initialize(const ProbabilisticTask& task)
{
    initialized = true;
    const int num_variables = task.get_variables().size();
    const int max_factor_index = 2 * num_variables - 1;
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

} // namespace probfd::merge_and_shrink