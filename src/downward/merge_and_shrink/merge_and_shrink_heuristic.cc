#include "downward/merge_and_shrink/merge_and_shrink_heuristic.h"

#include "downward/merge_and_shrink/distances.h"
#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_and_shrink_algorithm.h"
#include "downward/merge_and_shrink/merge_and_shrink_representation.h"
#include "downward/merge_and_shrink/transition_system.h"
#include "downward/merge_and_shrink/types.h"

#include "downward/task_utils/task_properties.h"
#include "downward/utils/markup.h"
#include "downward/utils/system.h"

#include "downward/task_transformation.h"

#include <cassert>
#include <iostream>
#include <utility>

using namespace std;
using downward::utils::ExitCode;

namespace downward::merge_and_shrink {

MergeAndShrinkHeuristic::MergeAndShrinkHeuristic(
    const shared_ptr<MergeStrategyFactory>& merge_strategy,
    const shared_ptr<ShrinkStrategy>& shrink_strategy,
    const shared_ptr<LabelReduction>& label_reduction,
    bool prune_unreachable_states,
    bool prune_irrelevant_states,
    int max_states,
    int max_states_before_merge,
    int threshold_before_merge,
    double main_loop_max_time,
    SharedAbstractTask original_task,
    TaskTransformationResult transformation_result,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(
          std::move(original_task),
          std::move(transformation_result),
          cache_estimates,
          description,
          verbosity)
{
    log << "Initializing merge-and-shrink heuristic..." << endl;
    MergeAndShrinkAlgorithm algorithm(
        merge_strategy,
        shrink_strategy,
        label_reduction,
        prune_unreachable_states,
        prune_irrelevant_states,
        max_states,
        max_states_before_merge,
        threshold_before_merge,
        main_loop_max_time,
        verbosity);
    FactoredTransitionSystem fts =
        algorithm.build_factored_transition_system(to_refs(transformed_task));
    extract_factors(fts);
    log << "Done initializing merge-and-shrink heuristic." << endl << endl;
}

MergeAndShrinkHeuristic::MergeAndShrinkHeuristic(
    const std::shared_ptr<MergeStrategyFactory>& merge_strategy,
    const std::shared_ptr<ShrinkStrategy>& shrink_strategy,
    const std::shared_ptr<LabelReduction>& label_reduction,
    bool prune_unreachable_states,
    bool prune_irrelevant_states,
    int max_states,
    int max_states_before_merge,
    int threshold_before_merge,
    double main_loop_max_time,
    SharedAbstractTask original_task,
    const std::shared_ptr<TaskTransformation>& transformation,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : MergeAndShrinkHeuristic(
          merge_strategy,
          shrink_strategy,
          label_reduction,
          prune_unreachable_states,
          prune_irrelevant_states,
          max_states,
          max_states_before_merge,
          threshold_before_merge,
          main_loop_max_time,
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          description,
          verbosity)
{
}

MergeAndShrinkHeuristic::~MergeAndShrinkHeuristic() = default;

void MergeAndShrinkHeuristic::extract_factor(
    FactoredTransitionSystem& fts,
    int index)
{
    /*
      Extract the factor at the given index from the given factored transition
      system, compute goal distances if necessary and store the M&S
      representation, which serves as the heuristic.
    */
    auto final_entry = fts.extract_factor(index);
    unique_ptr<MergeAndShrinkRepresentation> mas_representation =
        std::move(final_entry.mas_representation);
    unique_ptr<Distances> distances = std::move(final_entry.distances);
    if (!distances->are_goal_distances_computed()) {
        const bool compute_init = false;
        const bool compute_goal = true;
        distances->compute_distances(compute_init, compute_goal, log);
    }
    assert(distances->are_goal_distances_computed());
    mas_representation->set_distances(*distances);
    mas_representations.push_back(std::move(mas_representation));
}

bool MergeAndShrinkHeuristic::extract_unsolvable_factor(
    FactoredTransitionSystem& fts)
{
    /* Check if there is an unsolvable factor. If so, extract and store it and
       return true. Otherwise, return false. */
    for (int index : fts) {
        if (!fts.is_factor_solvable(index)) {
            mas_representations.reserve(1);
            extract_factor(fts, index);
            if (log.is_at_least_normal()) {
                log << fts.get_transition_system(index).tag()
                    << "use this unsolvable factor as heuristic." << endl;
            }
            return true;
        }
    }
    return false;
}

void MergeAndShrinkHeuristic::extract_nontrivial_factors(
    FactoredTransitionSystem& fts)
{
    // Iterate over remaining factors and extract and store the nontrivial ones.
    for (int index : fts) {
        if (fts.is_factor_trivial(index)) {
            if (log.is_at_least_verbose()) {
                log << fts.get_transition_system(index).tag() << "is trivial."
                    << endl;
            }
        } else {
            extract_factor(fts, index);
        }
    }
}

void MergeAndShrinkHeuristic::extract_factors(FactoredTransitionSystem& fts)
{
    /*
      TODO: This method has quite a bit of fiddling with aspects of
      transition systems and the merge-and-shrink representation (checking
      whether distances have been computed; computing them) that we would
      like to have at a lower level. See also the TODO in
      factored_transition_system.h on improving the interface of that class
      (and also related classes like TransitionSystem etc).
    */
    assert(mas_representations.empty());

    int num_active_factors = fts.get_num_active_entries();
    if (log.is_at_least_normal()) {
        log << "Number of remaining factors: " << num_active_factors << endl;
    }

    bool unsolvalbe = extract_unsolvable_factor(fts);
    if (!unsolvalbe) {
        extract_nontrivial_factors(fts);
    }

    int num_factors_kept = mas_representations.size();
    if (log.is_at_least_normal()) {
        log << "Number of factors kept: " << num_factors_kept << endl;
    }
}

int MergeAndShrinkHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    int heuristic = 0;
    for (const unique_ptr<MergeAndShrinkRepresentation>& mas_representation :
         mas_representations) {
        int cost = mas_representation->get_value(state);
        if (cost == PRUNED_STATE || cost == INF) {
            // If state is unreachable or irrelevant, we encountered a dead end.
            return DEAD_END;
        }
        heuristic = max(heuristic, cost);
    }
    return heuristic;
}

} // namespace merge_and_shrink
