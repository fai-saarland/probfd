#include "probfd/heuristics/merge_and_shrink_heuristic.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_and_shrink_algorithm.h"
#include "probfd/merge_and_shrink/merge_and_shrink_representation.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/task_evaluator_factory.h"

#include "downward/plugins/plugin.h"
#include "downward/utils/system.h"

#include <cassert>
#include <iostream>
#include <utility>

using namespace std;
using utils::ExitCode;
using namespace probfd::merge_and_shrink;

namespace probfd::heuristics {

MergeAndShrinkHeuristic::MergeAndShrinkHeuristic(
    MergeAndShrinkAlgorithm& algorithm,
    std::shared_ptr<ProbabilisticTask> task,
    utils::LogProxy log)
    : TaskDependentHeuristic(task, std::move(log))
{
    log_ << "Initializing merge-and-shrink heuristic..." << endl;
    FactoredTransitionSystem fts =
        algorithm.build_factored_transition_system(task);
    extract_factors(fts);
    log_ << "Done initializing merge-and-shrink heuristic." << endl << endl;
}

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
        std::move(final_entry.first);
    unique_ptr<Distances> distances = std::move(final_entry.second);
    if (!distances->are_goal_distances_computed()) {
        distances->compute_distances(false, log_);
    }
    assert(distances->are_goal_distances_computed());
    mas_representations.push_back(
        mas_representation->create_distance_representation(*distances));
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
            if (log_.is_at_least_normal()) {
                log_ << fts.get_transition_system(index).tag()
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
            if (log_.is_at_least_verbose()) {
                log_ << fts.get_transition_system(index).tag() << "is trivial."
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
    if (log_.is_at_least_normal()) {
        log_ << "Number of remaining factors: " << num_active_factors << endl;
    }

    bool unsolvable = extract_unsolvable_factor(fts);
    if (!unsolvable) {
        extract_nontrivial_factors(fts);
    }

    int num_factors_kept = mas_representations.size();
    if (log_.is_at_least_normal()) {
        log_ << "Number of factors kept: " << num_factors_kept << endl;
    }
}

value_t MergeAndShrinkHeuristic::evaluate(const State& state) const
{
    value_t heuristic = 0;
    for (const auto& mas_representation : mas_representations) {
        value_t cost = mas_representation->get_abstract_distance(state);
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

public:
    explicit MergeAndShrinkHeuristicFactory(const plugins::Options& opts);

    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

MergeAndShrinkHeuristicFactory::MergeAndShrinkHeuristicFactory(
    const plugins::Options& opts)
    : algorithm(opts)
    , log_(utils::get_log_from_options(opts))
{
}

std::unique_ptr<FDREvaluator> MergeAndShrinkHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction>)
{
    return std::make_unique<MergeAndShrinkHeuristic>(algorithm, task, log_);
}

class MergeAndShrinkHeuristicFactoryFeature
    : public plugins::
          TypedFeature<TaskEvaluatorFactory, MergeAndShrinkHeuristicFactory> {
public:
    MergeAndShrinkHeuristicFactoryFeature()
        : TypedFeature("pa_merge_and_shrink")
    {
        document_title("Merge-and-shrink heuristic");
        document_synopsis("TODO add a description");

        TaskDependentHeuristic::add_options_to_feature(*this);
        add_merge_and_shrink_algorithm_options_to_feature(*this);
    }

    virtual shared_ptr<MergeAndShrinkHeuristicFactory> create_component(
        const plugins::Options& options,
        const utils::Context& context) const override
    {
        plugins::Options options_copy(options);
        handle_shrink_limit_options_defaults(options_copy, context);
        return make_shared<MergeAndShrinkHeuristicFactory>(options_copy);
    }
};

static plugins::FeaturePlugin<MergeAndShrinkHeuristicFactoryFeature> _plugin;

} // namespace
} // namespace probfd::heuristics