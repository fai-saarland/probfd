#include "downward/cli/plugins/plugin.h"

#include "downward/cli/merge_and_shrink/merge_strategy_options.h"

#include "downward/merge_and_shrink/merge_strategy_factory_sccs.h"

#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::merge_and_shrink::add_merge_strategy_options_to_feature;
using downward::cli::merge_and_shrink::
    get_merge_strategy_arguments_from_options;

namespace {

class MergeStrategyFactorySCCsFeature
    : public TypedFeature<MergeStrategyFactory, MergeStrategyFactorySCCs> {
public:
    MergeStrategyFactorySCCsFeature()
        : TypedFeature("merge_sccs")
    {
        document_title("Merge strategy SSCs");
        document_synopsis(
            "This merge strategy implements the algorithm described in the "
            "paper " +
            format_conference_reference(
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

        add_option<OrderOfSCCs>(
            "order_of_sccs",
            "how the SCCs should be ordered",
            "topological");
        add_option<shared_ptr<MergeSelector>>(
            "merge_selector",
            "the fallback merge strategy to use.");
        add_merge_strategy_options_to_feature(*this);
    }

    virtual shared_ptr<MergeStrategyFactorySCCs>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<MergeStrategyFactorySCCs>(
            opts.get<OrderOfSCCs>("order_of_sccs"),
            opts.get<shared_ptr<MergeSelector>>("merge_selector"),
            get_merge_strategy_arguments_from_options(opts));
    }
};

FeaturePlugin<MergeStrategyFactorySCCsFeature> _plugin;

TypedEnumPlugin<OrderOfSCCs> _enum_plugin(
    {{"topological",
      "according to the topological ordering of the directed graph "
      "where each obtained SCC is a 'supervertex'"},
     {"reverse_topological",
      "according to the reverse topological ordering of the directed "
      "graph where each obtained SCC is a 'supervertex'"},
     {"decreasing", "biggest SCCs first, using 'topological' as tie-breaker"},
     {"increasing",
      "smallest SCCs first, using 'topological' as tie-breaker"}});

} // namespace
