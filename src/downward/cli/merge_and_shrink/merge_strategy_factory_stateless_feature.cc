#include "downward/cli/plugins/plugin.h"

#include "downward/cli/merge_and_shrink/merge_strategy_options.h"

#include "downward/merge_and_shrink/merge_strategy_factory_stateless.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::merge_and_shrink::add_merge_strategy_options_to_feature;
using downward::cli::merge_and_shrink::
    get_merge_strategy_arguments_from_options;

namespace {

class MergeStrategyFactoryStatelessFeature
    : public TypedFeature<MergeStrategyFactory, MergeStrategyFactoryStateless> {
public:
    MergeStrategyFactoryStatelessFeature()
        : TypedFeature("merge_stateless")
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

    virtual shared_ptr<MergeStrategyFactoryStateless>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<MergeStrategyFactoryStateless>(
            opts.get<shared_ptr<MergeSelector>>("merge_selector"),
            get_merge_strategy_arguments_from_options(opts));
    }
};

FeaturePlugin<MergeStrategyFactoryStatelessFeature> _plugin;

} // namespace
