#include "probfd/merge_and_shrink/merge_strategy_factory_sccs.h"

#include "probfd/merge_and_shrink/merge_selector.h"
#include "probfd/merge_and_shrink/merge_strategy_sccs.h"
#include "probfd/merge_and_shrink/merge_tree_factory.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/task_utils/causal_graph.h"

#include "probfd/task_proxy.h"

#include "downward/algorithms/sccs.h"

#include "downward/utils/logging.h"

#include <algorithm>
#include <cassert>

using namespace std;

namespace probfd::merge_and_shrink {

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
    utils::Verbosity verbosity,
    OrderOfSCCs order_of_sccs,
    std::shared_ptr<MergeTreeFactory> merge_tree_factory,
    std::shared_ptr<MergeSelector> merge_selector)
    : MergeStrategyFactory(verbosity)
    , order_of_sccs(order_of_sccs)
    , merge_tree_factory(merge_tree_factory)
    , merge_selector(merge_selector)
{
}

unique_ptr<MergeStrategy> MergeStrategyFactorySCCs::compute_merge_strategy(
    std::shared_ptr<ProbabilisticTask>& task,
    const FactoredTransitionSystem& fts)
{
    ProbabilisticTaskProxy task_proxy(*task);

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
        task,
        merge_tree_factory,
        merge_selector,
        std::move(non_singleton_cg_sccs));
}

bool MergeStrategyFactorySCCs::requires_liveness() const
{
    if (merge_tree_factory) {
        return merge_tree_factory->requires_liveness();
    } else {
        return merge_selector->requires_liveness();
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

} // namespace probfd::merge_and_shrink
