#include "probfd/cli/merge_and_shrink/merge_strategy_factory_stateless.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/merge_and_shrink/merge_strategy_factory_stateless.h"

#include "probfd/merge_and_shrink/merge_selector.h"
#include "probfd/merge_and_shrink/merge_strategy_stateless.h"

#include "probfd/cli/merge_and_shrink/merge_strategy_factory_options.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace {
class MergeStrategyFactoryStatelessFeature
    : public TypedFeature<MergeStrategyFactory> {
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
    shared_ptr<MergeStrategyFactory>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MergeStrategyFactoryStateless>(
            get_merge_strategy_args_from_options(options),
            options.get<shared_ptr<MergeSelector>>("merge_selector"));
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_stateless_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<MergeStrategyFactoryStatelessFeature>();
}

} // namespace probfd::cli::merge_and_shrink
