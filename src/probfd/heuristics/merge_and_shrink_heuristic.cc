#include "probfd/heuristics/merge_and_shrink_heuristic.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_mapping.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_and_shrink_algorithm.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/task_heuristic_factory.h"

#include "downward/utils/system.h"

#include <cassert>
#include <iostream>

using namespace std;
using utils::ExitCode;
using namespace probfd::merge_and_shrink;

using namespace downward::cli::plugins;

namespace probfd::heuristics {

struct MergeAndShrinkHeuristic::FactorDistances {
    std::unique_ptr<FactoredMapping> factored_mapping;
    std::vector<value_t> distance_table;

    FactorDistances(
        std::unique_ptr<merge_and_shrink::FactoredMapping> factored_mapping,
        Distances& distances)
        : factored_mapping(std::move(factored_mapping))
        , distance_table(distances.extract_goal_distances())
    {
    }

    FactorDistances(
        std::unique_ptr<merge_and_shrink::FactoredMapping> factored_mapping,
        TransitionSystem& ts)
        : factored_mapping(std::move(factored_mapping))
        , distance_table(ts.get_size(), -INFINITE_VALUE)
    {
        compute_goal_distances(ts, distance_table);
    }

    value_t lookup_distance(const State& state) const
    {
        return distance_table[factored_mapping->get_abstract_state(state)];
    }
};

MergeAndShrinkHeuristic::MergeAndShrinkHeuristic(
    MergeAndShrinkAlgorithm& algorithm,
    std::shared_ptr<ProbabilisticTask> task,
    utils::LogProxy log)
    : TaskDependentHeuristic(std::move(task), std::move(log))
{
    log_ << "Initializing merge-and-shrink heuristic..." << endl;
    FactoredTransitionSystem fts =
        algorithm.build_factored_transition_system(task);

    /*
      TODO: This method has quite a bit of fiddling with aspects of
      transition systems and the merge-and-shrink representation (checking
      whether distances have been computed; computing them) that we would
      like to have at a lower level. See also the TODO in
      factored_transition_system.h on improving the interface of that class
      (and also related classes like TransitionSystem etc).
    */
    assert(factor_distances.empty());

    int num_active_factors = fts.get_num_active_entries();
    if (log_.is_at_least_normal()) {
        log_ << "Number of remaining factors: " << num_active_factors << endl;
    }

    bool unsolvable = extract_unsolvable_factor(fts);
    if (!unsolvable) {
        // Iterate over remaining factors and extract and store the nontrivial
        // ones.
        for (int index : fts) {
            if (fts.is_factor_trivial(index)) {
                if (log_.is_at_least_verbose()) {
                    log_ << fts.get_transition_system(index).tag()
                         << "is trivial." << endl;
                }
                continue;
            }

            extract_factor(fts, index);
        }
    }

    int num_factors_kept = factor_distances.size();
    if (log_.is_at_least_normal()) {
        log_ << "Number of factors kept: " << num_factors_kept << endl;
    }

    log_ << "Done initializing merge-and-shrink heuristic." << endl << endl;
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
    auto&& [ts, fm, distances] = fts.extract_factor(index);

    if (distances->are_goal_distances_computed()) {
        factor_distances.emplace_back(std::move(fm), *distances);
    } else {
        factor_distances.emplace_back(std::move(fm), *ts);
    }
}

bool MergeAndShrinkHeuristic::extract_unsolvable_factor(
    FactoredTransitionSystem& fts)
{
    /* Check if there is an unsolvable factor. If so, extract and store it and
       return true. Otherwise, return false. */
    for (const int index : fts) {
        if (!fts.is_factor_solvable(index)) {
            if (log_.is_at_least_normal()) {
                log_ << fts.get_transition_system(index).tag()
                     << "use this unsolvable factor as heuristic." << endl;
            }
            factor_distances.reserve(1);
            extract_factor(fts, index);
            return true;
        }
    }
    return false;
}

value_t MergeAndShrinkHeuristic::evaluate(const State& state) const
{
    value_t heuristic = 0;
    for (const auto& distances : factor_distances) {
        value_t cost = distances.lookup_distance(state);
        if (cost == INFINITE_VALUE) {
            // If state is unreachable or irrelevant, we encountered a dead end.
            return INFINITE_VALUE;
        }
        heuristic = max(heuristic, cost);
    }
    return heuristic;
}

namespace {

class MergeAndShrinkHeuristicFactory : public TaskEvaluatorFactory {
    MergeAndShrinkAlgorithm algorithm;
    const utils::LogProxy log_;

} // namespace probfd::heuristics