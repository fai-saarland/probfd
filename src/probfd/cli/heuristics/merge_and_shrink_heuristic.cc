#include "probfd/cli/heuristics/merge_and_shrink_heuristic.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/cli/merge_and_shrink/merge_and_shrink_algorithm_options.h"

#include "probfd/heuristics/merge_and_shrink_heuristic.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_mapping.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_and_shrink_algorithm.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/task_heuristic_factory.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward;
using namespace probfd;
using namespace probfd::heuristics;
using namespace probfd::merge_and_shrink;

using namespace language::plugins;
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
        utils::FSeconds main_loop_max_time,
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
    utils::FSeconds main_loop_max_time,
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

    log_.println("Initializing merge-and-shrink heuristic...");

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
        log_.println("Number of remaining factors: {}", num_active_factors);
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
        log_.println("Number of factors kept: {}", num_factors_kept);
    }

    log_.println("Done initializing merge-and-shrink heuristic.\n");

    return std::make_unique<MergeAndShrinkHeuristic>(
        std::move(factor_distances));
}

shared_ptr<TaskHeuristicFactory> create_merge_and_shrink_with_lr(
    std::shared_ptr<MergeStrategyFactory> merge_strategy,
    std::shared_ptr<ShrinkStrategy> shrink_strategy,
    std::shared_ptr<LabelReduction> label_reduction,
    std::shared_ptr<PruneStrategy> prune_strategy,
    int max_states,
    int max_states_before_merge,
    int threshold_before_merge,
    utils::FSeconds main_loop_max_time,
    utils::Verbosity verbosity)
{
    handle_shrink_limit_options_defaults(
        max_states,
        max_states_before_merge,
        threshold_before_merge);

    return make_shared_from_arg_tuples<MergeAndShrinkHeuristicFactory>(
        std::move(merge_strategy),
        std::move(shrink_strategy),
        std::move(label_reduction),
        std::move(prune_strategy),
        max_states,
        max_states_before_merge,
        threshold_before_merge,
        main_loop_max_time,
        verbosity);
}

shared_ptr<TaskHeuristicFactory> create_merge_and_shrink_no_lr(
    std::shared_ptr<MergeStrategyFactory> merge_strategy,
    std::shared_ptr<ShrinkStrategy> shrink_strategy,
    std::shared_ptr<PruneStrategy> prune_strategy,
    int max_states,
    int max_states_before_merge,
    int threshold_before_merge,
    utils::FSeconds main_loop_max_time,
    utils::Verbosity verbosity)
{
    handle_shrink_limit_options_defaults(
        max_states,
        max_states_before_merge,
        threshold_before_merge);

    return make_shared_from_arg_tuples<MergeAndShrinkHeuristicFactory>(
        std::move(merge_strategy),
        std::move(shrink_strategy),
        nullptr,
        std::move(prune_strategy),
        max_states,
        max_states_before_merge,
        threshold_before_merge,
        main_loop_max_time,
        verbosity);
}

InternalFunctionDefinitionBase& add_merge_and_shrink_with_lr_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pa_merge_and_shrink",
        &create_merge_and_shrink_with_lr);

    f.document_title("Merge-and-shrink heuristic");
    f.document_synopsis("TODO add a description");

    // Merge strategy option.
    f.make_required_argument(
        0,
        "merge_strategy",
        "See detailed documentation for merge strategies.");

    // Shrink strategy option.
    f.make_optional_argument_with_default(
        1,
        "shrink_strategy",
        "pshrink_random()",
        "See detailed documentation for shrink strategies.");

    // Pruning strategy option.
    f.make_optional_argument_with_default(
        2,
        "prune_strategy",
        "prune_identity()",
        "See detailed documentation for pruning strategies.");

    // Label reduction option.
    f.make_required_argument(
        3,
        "label_reduction",
        "See detailed documentation for labels. There is currently only "
        "one 'option' to use label_reduction, which is "
        "{{{label_reduction=exact()}}} "
        "Also note the interaction with shrink strategies.");

    const auto n =
        add_transition_system_size_limit_options_to_feature(f, 4);

    f.make_optional_argument_with_default(
        n + 4,
        "main_loop_max_time",
        "seconds_max()",
        "A limit in seconds on the runtime of the main loop of the "
        "algorithm. "
        "If the limit is exceeded, the algorithm terminates, potentially "
        "returning a factored transition system with several factors. Also "
        "note that the time limit is only checked between transformations "
        "of the main loop, but not during, so it can be exceeded if a "
        "transformation is runtime-intense.");

    add_task_dependent_heuristic_options_to_feature(f, n + 5);

    return f;
}

InternalFunctionDefinitionBase& add_merge_and_shrink_no_lr_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pa_merge_and_shrink_no_lr",
        &create_merge_and_shrink_no_lr);
    f.document_title("Merge-and-shrink heuristic");
    f.document_synopsis("TODO add a description");

    // Merge strategy option.
    f.make_required_argument(
        0,
        "merge_strategy",
        "See detailed documentation for merge strategies.");

    // Shrink strategy option.
    f.make_optional_argument_with_default(
        1,
        "shrink_strategy",
        "pshrink_random()",
        "See detailed documentation for shrink strategies.");

    // Pruning strategy option.
    f.make_optional_argument_with_default(
        2,
        "prune_strategy",
        "prune_identity()",
        "See detailed documentation for pruning strategies.");

    const auto n = add_transition_system_size_limit_options_to_feature(f, 3);

    f.make_optional_argument_with_default(
        n + 3,
        "main_loop_max_time",
        "seconds_max()",
        "A limit in seconds on the runtime of the main loop of the "
        "algorithm. "
        "If the limit is exceeded, the algorithm terminates, potentially "
        "returning a factored transition system with several factors. Also "
        "note that the time limit is only checked between transformations "
        "of the main loop, but not during, so it can be exceeded if a "
        "transformation is runtime-intense.");

    add_task_dependent_heuristic_options_to_feature(f, n + 4);

    return f;
}

} // namespace

namespace probfd::cli::heuristics {

void add_merge_and_shrink_heuristic_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_merge_and_shrink_with_lr_to_namespace(n);
    add_merge_and_shrink_no_lr_to_namespace(n);
}

} // namespace probfd::cli::heuristics
