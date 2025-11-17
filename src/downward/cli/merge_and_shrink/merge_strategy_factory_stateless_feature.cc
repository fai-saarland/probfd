#include "downward/cli/merge_and_shrink/merge_strategy_factory_stateless_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/merge_and_shrink/merge_strategy_options.h"

#include "downward/merge_and_shrink/merge_strategy_factory_stateless.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::merge_and_shrink::add_merge_strategy_options_to_feature;

namespace {
class MergeStrategyFactoryStatelessFeature
    : public SharedTypedFeature<
          MergeStrategyFactory,
          const std::shared_ptr<MergeSelector>&,
          downward::utils::Verbosity> {
public:
    MergeStrategyFactoryStatelessFeature()
        : TypedFeature(
              "merge_stateless",
              &MergeStrategyFactoryStatelessFeature::func)
    {
        document_title("Stateless merge strategy");
        document_synopsis(
            "This merge strategy has a merge selector, which computes the next "
            "merge only depending on the current state of the factored "
            "transition "
            "system, not requiring any additional information.");

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

        make_required_argument(
            0,
            "merge_selector",
            "The merge selector to be used.");
        add_merge_strategy_options_to_feature(*this, 1);
    }

    static shared_ptr<MergeStrategyFactory> func(
        const Context&,
        const std::shared_ptr<MergeSelector>& merge_selector,
        downward::utils::Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<MergeStrategyFactoryStateless>(
            merge_selector,
            verbosity);
    }
};
} // namespace

namespace downward::cli::merge_and_shrink {

void add_merge_strategy_factory_stateless_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<MergeStrategyFactoryStatelessFeature>();
}

} // namespace downward::cli::merge_and_shrink
