#include "probfd/cli/merge_and_shrink/merge_strategy_factory_stateless.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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
    : public SharedTypedFeature<
          MergeStrategyFactory,
          utils::Verbosity,
          std::shared_ptr<MergeSelector>> {
public:
    MergeStrategyFactoryStatelessFeature()
        : TypedFeature(
              "pmerge_stateless",
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

        const auto n = add_merge_strategy_options_to_feature(*this, 0);
        make_required_argument(
            n,
            "merge_selector",
            "The merge selector to be used.");
    }

protected:
    static shared_ptr<MergeStrategyFactory> func(
        const utils::Context&,
        utils::Verbosity verbosity,
        std::shared_ptr<MergeSelector> merge_selector)
    {
        return make_shared_from_arg_tuples<MergeStrategyFactoryStateless>(
            verbosity,
            std::move(merge_selector));
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_stateless_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<MergeStrategyFactoryStatelessFeature>();
}

} // namespace probfd::cli::merge_and_shrink
