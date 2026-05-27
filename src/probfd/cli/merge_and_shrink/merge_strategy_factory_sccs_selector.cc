#include "probfd/cli/merge_and_shrink/merge_strategy_factory_sccs_selector.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/merge_and_shrink/merge_strategy_factory_sccs_selector.h"

#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace language::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace {

InternalFunctionDefinitionBase&
add_merge_strategy_sccs_selector_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_function_definition(
        "pmerge_sccs_selector",
        &language::plugins::construct_shared<
            MergeStrategyFactory,
            MergeStrategyFactorySCCsSelector,
            MergeStrategyFactorySCCsSelector::OrderOfSCCs,
            std::shared_ptr<MergeSelectorFactory>>);

    f.document_title("Merge strategy SSCs");
    f.document_synopsis(
        "This merge strategy implements the algorithm described in the "
        "paper " +
        utils::format_conference_reference(
            {"Silvan Sievers", "Martin Wehrle", "Malte Helmert"},
            "An Analysis of Merge Strategies for Merge-and-Shrink "
            "Heuristics",
            "https://ai.dmi.unibas.ch/papers/sievers-et-al-icaps2016.pdf",
            "Proceedings of the 26th International Conference on Planning "
            "and "
            "Scheduling (ICAPS 2016)",
            "2358-2366",
            "AAAI Press",
            "2016") +
        "In a nutshell, it computes the maximal SCCs of the causal graph, "
        "obtaining a partitioning of the task's variables. Every such "
        "partition is then merged individually, using the specified "
        "fallback "
        "merge strategy, considering the SCCs in a configurable order. "
        "Afterwards, all resulting composite abstractions are merged to "
        "form "
        "the final abstraction, again using the specified fallback merge "
        "strategy and the configurable order of the SCCs.");

    f.make_optional_argument_with_default(
        0,
        "order_of_sccs",
        "topological",
        "how the SCCs should be ordered");
    f.make_required_argument(
        1,
        "merge_selector",
        "the fallback merge strategy to use if a stateless strategy should "
        "be used.");

    return f;
}

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_sccs_selector_feature(Namespace& nspace)
{
    nspace
        .insert_enum_declaration<MergeStrategyFactorySCCsSelector::OrderOfSCCs>(
            {{"topological",
              "according to the topological ordering of the directed graph "
              "where each obtained SCC is a 'supervertex'"},
             {"reverse_topological",
              "according to the reverse topological ordering of the directed "
              "graph where each obtained SCC is a 'supervertex'"},
             {"decreasing",
              "biggest SCCs first, using 'topological' as tie-breaker"},
             {"increasing",
              "smallest SCCs first, using 'topological' as tie-breaker"}});

    add_merge_strategy_sccs_selector_to_namespace(nspace);
}

} // namespace probfd::cli::merge_and_shrink
