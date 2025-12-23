#include "downward/cli/merge_and_shrink/merge_strategy_factory_precomputed_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/merge_and_shrink/merge_strategy_options.h"

#include "downward/merge_and_shrink/merge_strategy_factory_precomputed.h"

using namespace std;
using namespace downward::utils;
using namespace downward::merge_and_shrink;

using namespace language::plugins;

using downward::cli::merge_and_shrink::add_merge_strategy_options_to_feature;

namespace downward::cli::merge_and_shrink {

InternalFunctionDefinitionBase&
add_merge_strategy_factory_precomputed_feature(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "merge_precomputed",
        &language::plugins::construct_shared<
            MergeStrategyFactory,
            MergeStrategyFactoryPrecomputed,
            std::shared_ptr<MergeTreeFactory>,
            downward::utils::Verbosity>);
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

    f.make_required_argument(0, "merge_tree", "The precomputed merge tree.");
    add_merge_strategy_options_to_feature(f, 1);

    return f;
}

} // namespace downward::cli::merge_and_shrink
