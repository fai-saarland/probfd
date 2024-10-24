#include "downward/merge_and_shrink/merge_strategy_factory_sccs.h"

#include "downward/merge_and_shrink/merge_selector.h"
#include "downward/merge_and_shrink/merge_strategy_sccs.h"
#include "downward/merge_and_shrink/merge_tree_factory.h"
#include "downward/merge_and_shrink/transition_system.h"

#include "downward/task_proxy.h"

#include "downward/algorithms/sccs.h"
#include "downward/plugins/plugin.h"
#include "downward/task_utils/causal_graph.h"
#include "downward/utils/logging.h"
#include "downward/utils/markup.h"
#include "downward/utils/system.h"

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std;

namespace merge_and_shrink {
static bool
compare_sccs_increasing(const vector<int>& lhs, const vector<int>& rhs)
{
    return lhs.size() < rhs.size();
}

static bool
compare_sccs_decreasing(const vector<int>& lhs, const vector<int>& rhs)
{
    return lhs.size() > rhs.size();
}

MergeStrategyFactorySCCs::MergeStrategyFactorySCCs(
    const OrderOfSCCs& order_of_sccs,
    const shared_ptr<MergeTreeFactory>& merge_tree,
    const shared_ptr<MergeSelector>& merge_selector,
    utils::Verbosity verbosity)
    : MergeStrategyFactory(verbosity)
    , order_of_sccs(order_of_sccs)
    , merge_tree_factory(merge_tree)
    , merge_selector(merge_selector)
{
}

unique_ptr<MergeStrategy> MergeStrategyFactorySCCs::compute_merge_strategy(
    const TaskProxy& task_proxy,
    const FactoredTransitionSystem& fts)
{
    VariablesProxy vars = task_proxy.get_variables();
    int num_vars = vars.size();

    // Compute SCCs of the causal graph.
    vector<vector<int>> cg;
    cg.reserve(num_vars);
    for (VariableProxy var : vars) {
        const vector<int>& successors =
            task_proxy.get_causal_graph().get_successors(var.get_id());
        cg.push_back(successors);
    }
    vector<vector<int>> sccs(sccs::compute_maximal_sccs(cg));

    // Put the SCCs in the desired order.
    switch (order_of_sccs) {
    case OrderOfSCCs::TOPOLOGICAL:
        // SCCs are computed in topological order.
        break;
    case OrderOfSCCs::REVERSE_TOPOLOGICAL:
        // SCCs are computed in topological order.
        reverse(sccs.begin(), sccs.end());
        break;
    case OrderOfSCCs::DECREASING:
        sort(sccs.begin(), sccs.end(), compare_sccs_decreasing);
        break;
    case OrderOfSCCs::INCREASING:
        sort(sccs.begin(), sccs.end(), compare_sccs_increasing);
        break;
    }

    if (log.is_at_least_normal()) {
        log << "SCCs of the causal graph:" << endl;
    }

    vector<vector<int>> non_singleton_cg_sccs;
    vector<int> indices_of_merged_sccs;
    indices_of_merged_sccs.reserve(sccs.size());
    for (const vector<int>& scc : sccs) {
        if (log.is_at_least_normal()) {
            log << scc << endl;
        }
        int scc_size = scc.size();
        if (scc_size != 1) {
            non_singleton_cg_sccs.push_back(scc);
        }
    }
    if (log.is_at_least_normal() && sccs.size() == 1) {
        log << "Only one single SCC" << endl;
    }
    if (log.is_at_least_normal() && static_cast<int>(sccs.size()) == num_vars) {
        log << "Only singleton SCCs" << endl;
        assert(non_singleton_cg_sccs.empty());
    }

    if (merge_selector) {
        merge_selector->initialize(task_proxy);
    }

    return std::make_unique<MergeStrategySCCs>(
        fts,
        task_proxy,
        merge_tree_factory,
        merge_selector,
        std::move(non_singleton_cg_sccs));
}

bool MergeStrategyFactorySCCs::requires_init_distances() const
{
    if (merge_tree_factory) {
        return merge_tree_factory->requires_init_distances();
    } else {
        return merge_selector->requires_init_distances();
    }
}

bool MergeStrategyFactorySCCs::requires_goal_distances() const
{
    if (merge_tree_factory) {
        return merge_tree_factory->requires_goal_distances();
    } else {
        return merge_selector->requires_goal_distances();
    }
}

void MergeStrategyFactorySCCs::dump_strategy_specific_options() const
{
    if (log.is_at_least_normal()) {
        log << "Merge order of sccs: ";
        switch (order_of_sccs) {
        case OrderOfSCCs::TOPOLOGICAL: log << "topological"; break;
        case OrderOfSCCs::REVERSE_TOPOLOGICAL:
            log << "reverse topological";
            break;
        case OrderOfSCCs::DECREASING: log << "decreasing"; break;
        case OrderOfSCCs::INCREASING: log << "increasing"; break;
        }
        log << endl;

        log << "Merge strategy for merging within sccs: " << endl;
        if (merge_tree_factory) {
            merge_tree_factory->dump_options(log);
        }
        if (merge_selector) {
            merge_selector->dump_options(log);
        }
    }
}

string MergeStrategyFactorySCCs::name() const
{
    return "sccs";
}

class MergeStrategyFactorySCCsFeature
    : public plugins::
          TypedFeature<MergeStrategyFactory, MergeStrategyFactorySCCs> {
public:
    MergeStrategyFactorySCCsFeature()
        : TypedFeature("merge_sccs")
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

        add_option<OrderOfSCCs>(
            "order_of_sccs",
            "how the SCCs should be ordered",
            "topological");
        add_option<shared_ptr<MergeTreeFactory>>(
            "merge_tree",
            "the fallback merge strategy to use if a precomputed strategy "
            "should "
            "be used.",
            plugins::ArgumentInfo::NO_DEFAULT);
        add_option<shared_ptr<MergeSelector>>(
            "merge_selector",
            "the fallback merge strategy to use if a stateless strategy should "
            "be used.",
            plugins::ArgumentInfo::NO_DEFAULT);
        add_merge_strategy_options_to_feature(*this);
    }

    virtual shared_ptr<MergeStrategyFactorySCCs> create_component(
        const plugins::Options& opts,
        const utils::Context& context) const override
    {
        bool merge_tree = opts.contains("merge_tree");
        bool merge_selector = opts.contains("merge_selector");
        if ((merge_tree && merge_selector) ||
            (!merge_tree && !merge_selector)) {
            context.error(
                "You have to specify exactly one of the options merge_tree "
                "and merge_selector!");
        }
        return plugins::make_shared_from_arg_tuples<MergeStrategyFactorySCCs>(
            opts.get<OrderOfSCCs>("order_of_sccs"),
            opts.get<shared_ptr<MergeTreeFactory>>("merge_tree", nullptr),
            opts.get<shared_ptr<MergeSelector>>("merge_selector", nullptr),
            get_merge_strategy_arguments_from_options(opts));
    }
};

static plugins::FeaturePlugin<MergeStrategyFactorySCCsFeature> _plugin;

static plugins::TypedEnumPlugin<OrderOfSCCs> _enum_plugin(
    {{"topological",
      "according to the topological ordering of the directed graph "
      "where each obtained SCC is a 'supervertex'"},
     {"reverse_topological",
      "according to the reverse topological ordering of the directed "
      "graph where each obtained SCC is a 'supervertex'"},
     {"decreasing", "biggest SCCs first, using 'topological' as tie-breaker"},
     {"increasing",
      "smallest SCCs first, using 'topological' as tie-breaker"}});
} // namespace merge_and_shrink
