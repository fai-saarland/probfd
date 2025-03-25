#include "probfd/heuristics/merge_and_shrink_heuristic.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_and_shrink_algorithm.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/task_heuristic_factory.h"

#include "probfd/cli/heuristics/task_dependent_heuristic_options.h"

#include "probfd/cli/merge_and_shrink/merge_and_shrink_algorithm_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace probfd;
using namespace probfd::heuristics;
using namespace probfd::merge_and_shrink;

using namespace downward::cli::plugins;
using namespace probfd::cli::heuristics;
using namespace probfd::cli::merge_and_shrink;

namespace {

class MergeAndShrinkHeuristicFactory : public TaskHeuristicFactory {
    MergeAndShrinkAlgorithm algorithm;
    const utils::LogProxy log_;

public:
    explicit MergeAndShrinkHeuristicFactory(
        std::shared_ptr<MergeStrategyFactory> merge_strategy,
        std::shared_ptr<ShrinkStrategy> shrink_strategy,
        std::shared_ptr<LabelReduction> label_reduction,
        std::shared_ptr<PruneStrategy> prune_strategy,
        int max_states,
        int max_states_before_merge,
        int threshold_before_merge,
        bool prune_unreachable_states,
        bool prune_irrelevant_states,
        double main_loop_max_time,
        utils::Verbosity verbosity);

    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

MergeAndShrinkHeuristicFactory::MergeAndShrinkHeuristicFactory(
    std::shared_ptr<MergeStrategyFactory> merge_strategy,
    std::shared_ptr<ShrinkStrategy> shrink_strategy,
    std::shared_ptr<LabelReduction> label_reduction,
    std::shared_ptr<PruneStrategy> prune_strategy,
    int max_states,
    int max_states_before_merge,
    int threshold_before_merge,
    bool prune_unreachable_states,
    bool prune_irrelevant_states,
    double main_loop_max_time,
    utils::Verbosity verbosity)
    : algorithm(
          std::move(merge_strategy),
          std::move(shrink_strategy),
          std::move(label_reduction),
          std::move(prune_strategy),
          max_states,
          max_states_before_merge,
          threshold_before_merge,
          prune_unreachable_states,
          prune_irrelevant_states,
          main_loop_max_time)
    , log_(get_log_for_verbosity(verbosity))
{
}

std::unique_ptr<FDREvaluator> MergeAndShrinkHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction>)
{
    FactoredTransitionSystem fts =
        algorithm.build_factored_transition_system(task, log_);
    return std::make_unique<MergeAndShrinkHeuristic>(fts, task, log_);
}

class MergeAndShrinkHeuristicFactoryFeature
    : public TypedFeature<
          TaskHeuristicFactory,
          MergeAndShrinkHeuristicFactory> {
public:
    MergeAndShrinkHeuristicFactoryFeature()
        : TypedFeature("pa_merge_and_shrink")
    {
        document_title("Merge-and-shrink heuristic");
        document_synopsis("TODO add a description");

        add_task_dependent_heuristic_options_to_feature(*this);
        add_merge_and_shrink_algorithm_options_to_feature(*this);
    }

    shared_ptr<MergeAndShrinkHeuristicFactory>
    create_component(const Options& options, const utils::Context& context)
        const override
    {
        Options options_copy(options);
        handle_shrink_limit_options_defaults(options_copy, context);

        return make_shared_from_arg_tuples<MergeAndShrinkHeuristicFactory>(
            options_copy.get<shared_ptr<MergeStrategyFactory>>(
                "merge_strategy"),
            options_copy.get<shared_ptr<ShrinkStrategy>>("shrink_strategy"),
            options_copy.get<shared_ptr<LabelReduction>>(
                "label_reduction",
                nullptr),
            options_copy.get<shared_ptr<PruneStrategy>>("prune_strategy"),
            options_copy.get<int>("max_states"),
            options_copy.get<int>("max_states_before_merge"),
            options_copy.get<int>("threshold_before_merge"),
            options_copy.get<bool>("prune_unreachable_states"),
            options_copy.get<bool>("prune_irrelevant_states"),
            options_copy.get<double>("main_loop_max_time"),
            get_task_dependent_heuristic_arguments_from_options(options_copy));
    }
};

FeaturePlugin<MergeAndShrinkHeuristicFactoryFeature> _plugin;

} // namespace
