#include "probfd/merge_and_shrink/merge_strategy_factory_stateless.h"

#include "probfd/merge_and_shrink/merge_selector.h"
#include "probfd/merge_and_shrink/merge_strategy_stateless.h"

#include "probfd/task_proxy.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/utils/memory.h"

using namespace std;
using namespace downward::cli::plugins;

namespace probfd::merge_and_shrink {

MergeStrategyFactoryStateless::MergeStrategyFactoryStateless(
    utils::Verbosity verbosity,
    std::shared_ptr<MergeSelector> merge_selector)
    : MergeStrategyFactory(verbosity)
    , merge_selector(std::move(merge_selector))
{
}

unique_ptr<MergeStrategy> MergeStrategyFactoryStateless::compute_merge_strategy(
    std::shared_ptr<ProbabilisticTask>& task,
    const FactoredTransitionSystem& fts)
{
    const ProbabilisticTaskProxy task_proxy(*task);
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

bool MergeStrategyFactoryStateless::requires_init_distances() const
{
    return merge_selector->requires_init_distances();
}

bool MergeStrategyFactoryStateless::requires_goal_distances() const
{
    return merge_selector->requires_goal_distances();
}

namespace {

class MergeStrategyFactoryStatelessFeature
    : public TypedFeature<MergeStrategyFactory, MergeStrategyFactoryStateless> {
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

protected:
    shared_ptr<MergeStrategyFactoryStateless>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MergeStrategyFactoryStateless>(
            get_merge_strategy_args_from_options(options),
            options.get<shared_ptr<MergeSelector>>("merge_selector"));
    }
};

FeaturePlugin<MergeStrategyFactoryStatelessFeature> _plugin;

} // namespace

} // namespace probfd::merge_and_shrink