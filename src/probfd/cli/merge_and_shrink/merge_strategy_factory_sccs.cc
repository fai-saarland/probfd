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
class MergeStrategyFactorySCCsTreeFeature
    : public SharedTypedFeature<
          MergeStrategyFactory,
          utils::Verbosity,
          OrderOfSCCs,
          std::shared_ptr<MergeTreeFactory>> {
public:
    MergeStrategyFactorySCCsTreeFeature()
        : TypedFeature(
              "pmerge_sccs_tree",
              &MergeStrategyFactorySCCsTreeFeature::func)
    {
        document_title("Merge strategy SSCs");
        document_synopsis(
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

        const auto n = add_merge_strategy_options_to_feature(*this, 0);

        make_optional_argument_with_default(
            n,
            "order_of_sccs",
            "topological",
            "how the SCCs should be ordered");
        make_required_argument(
            n + 1,
            "merge_tree",
            "the fallback merge strategy to use if a precomputed strategy "
            "should be used.");
    }

    static shared_ptr<MergeStrategyFactory> func(
        const utils::Context&,
        utils::Verbosity verbosity,
        OrderOfSCCs order_of_sccs,
        std::shared_ptr<MergeTreeFactory> merge_tree_factory)
    {
        return make_shared_from_arg_tuples<MergeStrategyFactorySCCsTree>(
            verbosity,
            order_of_sccs,
            std::move(merge_tree_factory));
    }
};

class MergeStrategyFactorySCCsSelectorFeature
    : public SharedTypedFeature<
          MergeStrategyFactory,
          utils::Verbosity,
          OrderOfSCCs,
          std::shared_ptr<MergeSelector>> {
public:
    MergeStrategyFactorySCCsSelectorFeature()
        : TypedFeature(
              "pmerge_sccs_selector",
              &MergeStrategyFactorySCCsSelectorFeature::func)
    {
        document_title("Merge strategy SSCs");
        document_synopsis(
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

        const auto n = add_merge_strategy_options_to_feature(*this, 0);

        make_optional_argument_with_default(
            n,
            "order_of_sccs",
            "topological",
            "how the SCCs should be ordered");
        make_required_argument(
            n + 1,
            "merge_selector",
            "the fallback merge strategy to use if a stateless strategy should "
            "be used.");
    }

    static shared_ptr<MergeStrategyFactory> func(
        const utils::Context&,
        utils::Verbosity verbosity,
        OrderOfSCCs order_of_sccs,
        std::shared_ptr<MergeSelector> merge_selector)
    {
        return make_shared_from_arg_tuples<MergeStrategyFactorySCCsSelector>(
            verbosity,
            order_of_sccs,
            std::move(merge_selector));
    }
};

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

    n.insert_feature_plugin<MergeStrategyFactorySCCsTreeFeature>();
    n.insert_feature_plugin<MergeStrategyFactorySCCsSelectorFeature>();
}

} // namespace probfd::cli::merge_and_shrink
