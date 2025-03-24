#include "probfd/merge_and_shrink/merge_strategy_factory_precomputed.h"

#include "probfd/merge_and_shrink/merge_strategy_precomputed.h"
#include "probfd/merge_and_shrink/merge_tree_factory.h"

#include "probfd/cli/merge_and_shrink/merge_strategy_factory_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace {

class MergeStrategyFactoryPrecomputedFeature
    : public TypedFeature<
          MergeStrategyFactory,
          MergeStrategyFactoryPrecomputed> {
public:
    MergeStrategyFactoryPrecomputedFeature()
        : TypedFeature("pmerge_precomputed")
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

        add_option<shared_ptr<MergeTreeFactory>>(
            "merge_tree",
            "The precomputed merge tree.");

        add_merge_strategy_options_to_feature(*this);

        document_note(
            "Note",
            "An example of a precomputed merge startegy is a linear merge "
            "strategy, "
            "which can be obtained using:\n"
            "{{{\n"
            "merge_strategy=merge_precomputed(merge_tree=linear(<variable_"
            "order>))"
            "\n}}}");
    }

protected:
    shared_ptr<MergeStrategyFactoryPrecomputed>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MergeStrategyFactoryPrecomputed>(
            get_merge_strategy_args_from_options(options),
            options.get<shared_ptr<MergeTreeFactory>>("merge_tree"));
    }
};

FeaturePlugin<MergeStrategyFactoryPrecomputedFeature> _plugin;

} // namespace
