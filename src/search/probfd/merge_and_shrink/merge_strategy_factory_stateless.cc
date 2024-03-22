#include "probfd/merge_and_shrink/merge_strategy_factory_stateless.h"

#include "probfd/merge_and_shrink/merge_selector.h"
#include "probfd/merge_and_shrink/merge_strategy_stateless.h"

#include "probfd/task_proxy.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/memory.h"

using namespace std;

namespace probfd::merge_and_shrink {

MergeStrategyFactoryStateless::MergeStrategyFactoryStateless(
    const plugins::Options& options)
    : MergeStrategyFactory(options)
    , merge_selector(options.get<shared_ptr<MergeSelector>>("merge_selector"))
{
}

unique_ptr<MergeStrategy> MergeStrategyFactoryStateless::compute_merge_strategy(
    std::shared_ptr<ProbabilisticTask>& task,
    const FactoredTransitionSystem& fts)
{
    ProbabilisticTaskProxy task_proxy(*task);
    merge_selector->initialize(task_proxy);
    return std::make_unique<MergeStrategyStateless>(fts, merge_selector);
}

string MergeStrategyFactoryStateless::name() const
{
    return "stateless";
}

void MergeStrategyFactoryStateless::dump_strategy_specific_options() const
{
    if (log.is_at_least_normal()) {
        merge_selector->dump_options(log);
    }
}

bool MergeStrategyFactoryStateless::requires_liveness() const
{
    return merge_selector->requires_liveness();
}

bool MergeStrategyFactoryStateless::requires_goal_distances() const
{
    return merge_selector->requires_goal_distances();
}

class MergeStrategyFactoryStatelessFeature
    : public plugins::
          TypedFeature<MergeStrategyFactory, MergeStrategyFactoryStateless> {
public:
    MergeStrategyFactoryStatelessFeature()
        : TypedFeature("pmerge_stateless")
    {
        document_title("Stateless merge strategy");
        document_synopsis(
            "This merge strategy has a merge selector, which computes the next "
            "merge only depending on the current state of the factored "
            "transition "
            "system, not requiring any additional information.");

        add_option<shared_ptr<MergeSelector>>(
            "merge_selector",
            "The merge selector to be used.");
        add_merge_strategy_options_to_feature(*this);

        document_note(
            "Note",
            "Examples include the DFP merge strategy, which can be obtained "
            "using:\n"
            "{{{\n"
            "merge_strategy=merge_stateless(merge_selector=score_based_"
            "filtering("
            "scoring_functions=[goal_relevance,dfp,total_order(<order_option>))"
            "]))"
            "\n}}}\n"
            "and the (dynamic/score-based) MIASM strategy, which can be "
            "obtained "
            "using:\n"
            "{{{\n"
            "merge_strategy=merge_stateless(merge_selector=score_based_"
            "filtering("
            "scoring_functions=[sf_miasm(<shrinking_options>),total_order(<"
            "order_option>)]"
            "\n}}}");
    }
};

static plugins::FeaturePlugin<MergeStrategyFactoryStatelessFeature> _plugin;

} // namespace probfd::merge_and_shrink