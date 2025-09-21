#include "probfd/cli/heuristics/merge_and_shrink_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/cli/merge_and_shrink/merge_and_shrink_algorithm_options.h"

#include "probfd/heuristics/merge_and_shrink_heuristic.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_mapping.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_and_shrink_algorithm.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/task_heuristic_factory_category.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;
using namespace probfd;
using namespace probfd::heuristics;
using namespace probfd::merge_and_shrink;

using namespace downward::cli::plugins;
using namespace probfd::cli::heuristics;
using namespace probfd::cli::merge_and_shrink;

namespace {
void extract_factor(
    std::vector<FactorDistances>& factor_distances,
    FactoredTransitionSystem& fts,
    int index)
{
    /*
      Extract the factor at the given index from the given factored
      transition system, compute goal distances if necessary and store the
      M&S representation, which serves as the heuristic.
    */

    if (auto&& [ts, fm, distances] = fts.extract_factor(index);
        distances->are_goal_distances_computed()) {
        factor_distances.emplace_back(std::move(fm), *distances);
    } else {
        factor_distances.emplace_back(std::move(fm), fts.get_labels(), *ts);
    }
}

bool extract_unsolvable_factor(
    utils::LogProxy log,
    std::vector<FactorDistances>& factor_distances,
    FactoredTransitionSystem& fts)
{
    /* Check if there is an unsolvable factor. If so, extract and store it
       and return true. Otherwise, return false. */
    for (const int index : fts) {
        if (!fts.is_factor_solvable(index)) {
            if (log.is_at_least_normal()) {
                log << fts.get_transition_system(index).tag()
                    << "use this unsolvable factor as heuristic." << endl;
            }
            factor_distances.reserve(1);
            extract_factor(factor_distances, fts, index);
            return true;
        }
    }

    return false;
}

class MergeAndShrinkHeuristicFactory final : public TaskHeuristicFactory {
    MergeAndShrinkAlgorithm algorithm;
    utils::LogProxy log_;

public:
    explicit MergeAndShrinkHeuristicFactory(
        std::shared_ptr<MergeStrategyFactory> merge_strategy,
        std::shared_ptr<ShrinkStrategy> shrink_strategy,
        std::shared_ptr<LabelReduction> label_reduction,
        std::shared_ptr<PruneStrategy> prune_strategy,
        int max_states,
        int max_states_before_merge,
        int threshold_before_merge,
        utils::Duration main_loop_max_time,
        utils::Verbosity verbosity);

    std::unique_ptr<FDRHeuristic>
    create_object(const SharedProbabilisticTask& task) override;
};

MergeAndShrinkHeuristicFactory::MergeAndShrinkHeuristicFactory(
    std::shared_ptr<MergeStrategyFactory> merge_strategy,
    std::shared_ptr<ShrinkStrategy> shrink_strategy,
    std::shared_ptr<LabelReduction> label_reduction,
    std::shared_ptr<PruneStrategy> prune_strategy,
    int max_states,
    int max_states_before_merge,
    int threshold_before_merge,
    utils::Duration main_loop_max_time,
    utils::Verbosity verbosity)
    : algorithm(
          std::move(merge_strategy),
          std::move(shrink_strategy),
          std::move(label_reduction),
          std::move(prune_strategy),
          max_states,
          max_states_before_merge,
          threshold_before_merge,
          main_loop_max_time)
    , log_(get_log_for_verbosity(verbosity))
{
}

std::unique_ptr<FDRHeuristic> MergeAndShrinkHeuristicFactory::create_object(
    const SharedProbabilisticTask& task)
{
    FactoredTransitionSystem fts =
        algorithm.build_factored_transition_system(task, log_);

    log_ << "Initializing merge-and-shrink heuristic..." << endl;

    /*
      TODO: This method has quite a bit of fiddling with aspects of
      transition systems and the merge-and-shrink representation (checking
      whether distances have been computed; computing them) that we would
      like to have at a lower level. See also the TODO in
      factored_transition_system.h on improving the interface of that class
      (and also related classes like TransitionSystem etc).
    */

    std::vector<FactorDistances> factor_distances;

    const int num_active_factors = fts.get_num_active_entries();
    if (log_.is_at_least_normal()) {
        log_ << "Number of remaining factors: " << num_active_factors << endl;
    }

    if (const bool unsolvable =
            extract_unsolvable_factor(log_, factor_distances, fts);
        !unsolvable) {
        // Iterate over remaining factors and extract and store the nontrivial
        // ones.
        for (const int index : fts) {
            if (fts.is_factor_trivial(index)) {
                if (log_.is_at_least_verbose()) {
                    log_ << fts.get_transition_system(index).tag()
                         << "is trivial." << endl;
                }
                continue;
            }

            extract_factor(factor_distances, fts, index);
        }
    }

    const int num_factors_kept = factor_distances.size();
    if (log_.is_at_least_normal()) {
        log_ << "Number of factors kept: " << num_factors_kept << endl;
    }

    log_ << "Done initializing merge-and-shrink heuristic." << endl << endl;

    return std::make_unique<MergeAndShrinkHeuristic>(
        std::move(factor_distances));
}

class MergeAndShrinkHeuristicFactoryFeature final
    : public TypedFeature<TaskHeuristicFactory> {
public:
    MergeAndShrinkHeuristicFactoryFeature()
        : TypedFeature("pa_merge_and_shrink")
    {
        document_title("Merge-and-shrink heuristic");
        document_synopsis("TODO add a description");

        add_task_dependent_heuristic_options_to_feature(*this);
        add_merge_and_shrink_algorithm_options_to_feature(*this);
    }

    shared_ptr<TaskHeuristicFactory>
    create_component(const Options& options, const utils::Context& context)
        const override
    {
        Options options_copy(options);
        handle_shrink_limit_options_defaults(options_copy, context);

        return make_shared_from_arg_tuples<MergeAndShrinkHeuristicFactory>(
            get_merge_and_shrink_algorithm_arguments_from_options(options_copy),
            get_task_dependent_heuristic_arguments_from_options(options_copy));
    }
};
} // namespace

namespace probfd::cli::heuristics {

void add_merge_and_shrink_heuristic_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<MergeAndShrinkHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
