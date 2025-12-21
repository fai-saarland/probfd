#include "downward/cli/merge_and_shrink/merge_strategy_factory_stateless_feature.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "downward/cli/merge_and_shrink/merge_strategy_options.h"

#include "downward/merge_and_shrink/merge_strategy_factory_stateless.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace language::plugins;

using downward::cli::merge_and_shrink::add_merge_strategy_options_to_feature;

namespace {

InternalFunctionDefinitionBase& add_merge_strategy_factory_stateless_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "merge_stateless",
        &language::plugins::construct_shared<
            MergeStrategyFactory,
            MergeStrategyFactoryStateless,
            std::shared_ptr<MergeSelector>,
            Verbosity>);
    f.document_title("Stateless merge strategy");
    f.document_synopsis(
        "This merge strategy has a merge selector, which computes the next "
        "merge only depending on the current state of the factored "
        "transition "
        "system, not requiring any additional information.");

    f.document_note(
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

    f.make_required_argument(
        0,
        "merge_selector",
        "The merge selector to be used.");
    add_merge_strategy_options_to_feature(f, 1);

    return f;
}

} // namespace

namespace downward::cli::merge_and_shrink {

void add_merge_strategy_factory_stateless_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    add_merge_strategy_factory_stateless_to_namespace(n);
}

} // namespace downward::cli::merge_and_shrink
