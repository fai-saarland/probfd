#include "downward_cli/merge_and_shrink/merge_tree_factory_category.h"

#include "downward_cli/utils/rng_options.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/merge_and_shrink/merge_tree.h"
#include "downward/merge_and_shrink/merge_tree_factory.h"

#include "downward/utils/markup.h"
#include "downward/utils/rng.h"

using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace language;
using namespace language::plugins;

namespace downward::cli::merge_and_shrink {

namespace {
class MergeUpdateStrategyUseFirstFeature
    : public SharedTypedFeature<MergeUpdateStrategy> {
public:
    MergeUpdateStrategyUseFirstFeature()
        : TypedFeature("use_first")
    {
        document_title("First node survives");
    }

    std::shared_ptr<MergeUpdateStrategy>
    create_component(const Options&, const Context&) const override
    {
        return create_merge_update_strategy_use_first();
    }
};

class MergeUpdateStrategyUseSecondFeature
    : public SharedTypedFeature<MergeUpdateStrategy> {
public:
    MergeUpdateStrategyUseSecondFeature()
        : TypedFeature("use_second")
    {
        document_title("Second node survives");
    }

    std::shared_ptr<MergeUpdateStrategy>
    create_component(const Options&, const Context&) const override
    {
        return create_merge_update_strategy_use_second();
    }
};

class MergeUpdateStrategyUseRandomFeature
    : public SharedTypedFeature<MergeUpdateStrategy> {
public:
    MergeUpdateStrategyUseRandomFeature()
        : TypedFeature("use_random")
    {
        document_title("Random node survives");
        utils::add_rng_options_to_feature(*this);
    }

    std::shared_ptr<MergeUpdateStrategy>
    create_component(const Options& opts, const Context& context) const override
    {
        return create_merge_update_strategy_use_random(
            get<0>(utils::get_rng_arguments_from_options(context, opts)));
    }
};
} // namespace

void add_merge_tree_categories(RawRegistry& raw_registry)
{
    auto& category =
        raw_registry.insert_category_plugin<MergeTreeFactory>("MergeTree");
    category.document_synopsis(
        "This page describes the available merge trees that can be used to "
        "precompute a merge strategy, either for the entire task or a "
        "given "
        "subset of transition systems of a given factored transition "
        "system.\n"
        "Merge trees are typically used in the merge strategy of type "
        "'precomputed', but they can also be used as fallback merge "
        "strategies in "
        "'combined' merge strategies.");

    auto& update_category =
        raw_registry.insert_category_plugin<MergeUpdateStrategy>(
            "MergeUpdateStrategy");
    update_category.document_synopsis(
        "The merge update strategy controls which merge tree nodes "
        "survives when two siblings are merged.");

    raw_registry.insert_feature_plugin<MergeUpdateStrategyUseFirstFeature>();
    raw_registry.insert_feature_plugin<MergeUpdateStrategyUseSecondFeature>();
    raw_registry.insert_feature_plugin<MergeUpdateStrategyUseRandomFeature>();
}

} // namespace downward::cli::merge_and_shrink
