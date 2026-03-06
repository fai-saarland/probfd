#include "probfd/cli/merge_and_shrink/merge_strategy_factory_stateless.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/merge_and_shrink/merge_strategy_factory_stateless.h"

#include "probfd/merge_and_shrink/merge_selector.h"
#include "probfd/merge_and_shrink/merge_strategy_stateless.h"

#include "probfd/cli/merge_and_shrink/merge_strategy_factory_options.h"

using namespace std;
using namespace language::parser;
using namespace downward;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

InternalFunctionDefinitionBase& add_merge_strategy_factory_stateless_feature(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        MergeStrategyFactory,
        MergeStrategyFactoryStateless,
        utils::Verbosity,
        std::shared_ptr<MergeSelector>>>(nspace, "pmerge_stateless");

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

    const auto n = add_merge_strategy_options_to_feature(f, 0);
    f.make_required_argument(
        n,
        "merge_selector",
        "The merge selector to be used.");

    return f;
}

} // namespace probfd::cli::merge_and_shrink
