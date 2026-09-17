#include "probfd/merge_and_shrink/merge_strategy_factory_sccs_tree.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_strategy_sccs_tree.h"
#include "probfd/merge_and_shrink/merge_tree_factory.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/task_utils/causal_graph.h"

#include "probfd/probabilistic_task.h"

#include "downward/algorithms/sccs.h"
#include "downward/task_utils/causal_graph.h"

#include "downward/utils/logging.h"

#include "downward/variable_space.h"

#include <algorithm>
#include <cassert>
#include <utility>

using namespace std;
using namespace downward;

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

MergeStrategyFactorySCCsTree::MergeStrategyFactorySCCsTree(
    utils::Verbosity verbosity,
    OrderOfSCCs order_of_sccs,
    std::shared_ptr<MergeTreeFactory> merge_tree_factory)
    : MergeStrategyFactorySCCs(verbosity, order_of_sccs)
    , merge_tree_factory(std::move(merge_tree_factory))
{
}

unique_ptr<MergeStrategy> MergeStrategyFactorySCCsTree::compute_merge_strategy(
    const SharedProbabilisticTask& task,
    const FactoredTransitionSystem& fts)
{
    const causal_graph::ProbabilisticCausalGraph cgraph(fts);

    const std::size_t num_vars = fts.get_size();

    // Compute SCCs of the causal graph.
    vector<vector<int>> cg;
    cg.reserve(num_vars);
    for (int i = 0; std::cmp_not_equal(i, num_vars); ++i) {
        cg.push_back(cgraph.get_successors(i));
    }

    vector<vector<int>> sccs = sccs::compute_maximal_sccs(cg);

    // Put the SCCs in the desired order.
    switch (order_of_sccs) {
    case OrderOfSCCs::TOPOLOGICAL:
        // SCCs are computed in topological order.
        break;
    case OrderOfSCCs::REVERSE_TOPOLOGICAL:
        // SCCs are computed in topological order.
        ranges::reverse(sccs);
        break;
    case OrderOfSCCs::DECREASING:
        ranges::sort(sccs, compare_sccs_decreasing);
        break;
    case OrderOfSCCs::INCREASING:
        ranges::sort(sccs, compare_sccs_increasing);
        break;
    }

    if (log.is_at_least_normal()) {
        log.println("SCCs of the causal graph:");
    }

    vector<vector<int>> non_singleton_cg_sccs;
    vector<int> indices_of_merged_sccs;
    indices_of_merged_sccs.reserve(sccs.size());
    for (const vector<int>& scc : sccs) {
        if (log.is_at_least_normal()) {
            log.println("{}", scc);
        }
        if (scc.size() != 1) {
            non_singleton_cg_sccs.push_back(scc);
        }
    }

    if (log.is_at_least_normal() && sccs.size() == 1) {
        log.println("Only one single SCC");
    }

    if (log.is_at_least_normal() && sccs.size() == num_vars) {
        log.println("Only singleton SCCs");
        assert(non_singleton_cg_sccs.empty());
    }

    return std::make_unique<MergeStrategySCCsTree>(
        task,
        merge_tree_factory,
        std::move(non_singleton_cg_sccs));
}

bool MergeStrategyFactorySCCsTree::requires_liveness() const
{
    return merge_tree_factory->requires_liveness();
}

bool MergeStrategyFactorySCCsTree::requires_goal_distances() const
{
    return merge_tree_factory->requires_goal_distances();
}

void MergeStrategyFactorySCCsTree::dump_strategy_specific_options() const
{
    if (log.is_at_least_normal()) {
        log.print("Merge order of sccs: ");
        switch (order_of_sccs) {
        case OrderOfSCCs::TOPOLOGICAL: log.print("topological"); break;
        case OrderOfSCCs::REVERSE_TOPOLOGICAL:
            log.print("reverse topological");
            break;
        case OrderOfSCCs::DECREASING: log.print("decreasing"); break;
        case OrderOfSCCs::INCREASING: log.print("increasing"); break;
        }
        log.println();

        log.println("Merge strategy for merging within sccs: ");
        merge_tree_factory->dump_options(log);
    }
}

string MergeStrategyFactorySCCsTree::name() const
{
    return "sccs-tree";
}

} // namespace probfd::merge_and_shrink
