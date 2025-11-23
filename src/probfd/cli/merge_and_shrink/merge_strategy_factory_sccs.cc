#include "probfd/cli/merge_and_shrink/merge_strategy_factory_sccs.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/cli/utils/logging_options.h"

#include "probfd/merge_and_shrink/merge_strategy_factory_sccs.h"

#include "probfd/merge_and_shrink/merge_selector.h"
#include "probfd/merge_and_shrink/merge_strategy_sccs.h"
#include "probfd/merge_and_shrink/merge_tree_factory.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

#include "probfd/cli/merge_and_shrink/merge_strategy_factory_options.h"

using namespace std;
using namespace downward::cli::plugins;
using namespace downward;
using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

namespace {

Feature& add_merge_strategy_sccs_tree_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "pmerge_sccs_tree",
        &downward::cli::plugins::make_shared<
            MergeStrategyFactory,
            MergeStrategyFactorySCCsTree,
            utils::Verbosity,
            OrderOfSCCs,
            std::shared_ptr<MergeTreeFactory>>);

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

    const auto n = add_merge_strategy_options_to_feature(f, 0);

    f.make_optional_argument_with_default(
        n,
        "order_of_sccs",
        "topological",
        "how the SCCs should be ordered");
    f.make_required_argument(
        n + 1,
        "merge_tree",
        "the fallback merge strategy to use if a precomputed strategy "
        "should be used.");

    return f;
}

Feature& add_merge_strategy_sccs_selector_to_namespace(Namespace& nspace)
{
    auto& f = nspace.insert_typed_feature_plugin(
        "pmerge_sccs_selector",
        &downward::cli::plugins::make_shared<
            MergeStrategyFactory,
            MergeStrategyFactorySCCsSelector,
            utils::Verbosity,
            OrderOfSCCs,
            std::shared_ptr<MergeSelector>>);

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

    const auto n = add_merge_strategy_options_to_feature(f, 0);

    f.make_optional_argument_with_default(
        n,
        "order_of_sccs",
        "topological",
        "how the SCCs should be ordered");
    f.make_required_argument(
        n + 1,
        "merge_selector",
        "the fallback merge strategy to use if a stateless strategy should "
        "be used.");

    return f;
}

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_strategy_factory_sccs_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_plugin<OrderOfSCCs>(
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

    add_merge_strategy_sccs_tree_to_namespace(n);
    add_merge_strategy_sccs_selector_to_namespace(n);
}

} // namespace probfd::cli::merge_and_shrink
