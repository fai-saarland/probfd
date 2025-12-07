#include "probfd/cli/merge_and_shrink/merge_strategy_factory_precomputed.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

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

InternalFunctionDefinitionBase& add_merge_strategy_precomputed_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pmerge_precomputed",
        &downward::cli::plugins::construct_shared<
            MergeStrategyFactory,
            MergeStrategyFactoryPrecomputed,
            utils::Verbosity,
            std::shared_ptr<MergeTreeFactory>>);

    f.document_title("Precomputed merge strategy");
    f.document_synopsis(
        "This merge strategy has a precomputed merge tree. Note that this "
        "merge strategy does not take into account the current state of "
        "the factored transition system. This also means that this merge "
        "strategy relies on the factored transition system being "
        "synchronized "
        "with this merge tree, i.e. all merges are performed exactly as "
        "given "
        "by the merge tree.");

    f.document_note(
        "Note",
        "An example of a precomputed merge startegy is a linear merge "
        "strategy, "
        "which can be obtained using:\n"
        "{{{\n"
        "merge_strategy=merge_precomputed(merge_tree=linear(<variable_"
        "order>))"
        "\n}}}");

    const auto n = add_merge_strategy_options_to_feature(f, 0);
    f.make_required_argument(n, "merge_tree", "The precomputed merge tree.");

    return f;
}

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_precomputed_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_merge_strategy_precomputed_to_namespace(n);
}

} // namespace probfd::cli::merge_and_shrink
