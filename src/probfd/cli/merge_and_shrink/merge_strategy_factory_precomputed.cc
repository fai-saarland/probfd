#include "probfd/cli/merge_and_shrink/merge_strategy_factory_precomputed.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/merge_and_shrink/merge_strategy_factory_precomputed.h"

#include "probfd/merge_and_shrink/merge_strategy_precomputed.h"
#include "probfd/merge_and_shrink/merge_tree_factory.h"

#include "probfd/cli/merge_and_shrink/merge_strategy_factory_options.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace {
class MergeStrategyFactoryPrecomputedFeature
    : public SharedTypedFeature<
          MergeStrategyFactory,
          utils::Verbosity,
          std::shared_ptr<MergeTreeFactory>> {
public:
    MergeStrategyFactoryPrecomputedFeature()
        : TypedFeature(
              "pmerge_precomputed",
              &MergeStrategyFactoryPrecomputedFeature::func)
    {
        document_title("Precomputed merge strategy");
        document_synopsis(
            "This merge strategy has a precomputed merge tree. Note that this "
            "merge strategy does not take into account the current state of "
            "the factored transition system. This also means that this merge "
            "strategy relies on the factored transition system being "
            "synchronized "
            "with this merge tree, i.e. all merges are performed exactly as "
            "given "
            "by the merge tree.");

        document_note(
            "Note",
            "An example of a precomputed merge startegy is a linear merge "
            "strategy, "
            "which can be obtained using:\n"
            "{{{\n"
            "merge_strategy=merge_precomputed(merge_tree=linear(<variable_"
            "order>))"
            "\n}}}");

        const auto n = add_merge_strategy_options_to_feature(*this, 0);
        make_required_argument(n, "merge_tree", "The precomputed merge tree.");
    }

protected:
    static shared_ptr<MergeStrategyFactory> func(
        const utils::Context&,
        downward::utils::Verbosity verbosity,
        std::shared_ptr<MergeTreeFactory> merge_tree_factory)
    {
        return make_shared_from_arg_tuples<MergeStrategyFactoryPrecomputed>(
            verbosity,
            std::move(merge_tree_factory));
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_precomputed_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<MergeStrategyFactoryPrecomputedFeature>();
}

} // namespace probfd::cli::merge_and_shrink
