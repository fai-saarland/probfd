#include "downward/merge_and_shrink/merge_tree_factory_linear.h"

#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_tree.h"
#include "downward/merge_and_shrink/transition_system.h"

#include "downward/abstract_task.h"
#include "downward/state.h"

#include "downward/utils/rng.h"

#include <algorithm>

using namespace std;

namespace downward::merge_and_shrink {
MergeTreeFactoryLinear::MergeTreeFactoryLinear(
    variable_order_finder::VariableOrderType variable_order,
    int random_seed,
    UpdateOption update_option)
    : MergeTreeFactory(random_seed, update_option)
    , variable_order_type(variable_order)
{
}

unique_ptr<MergeTree>
MergeTreeFactoryLinear::compute_merge_tree(const AbstractTask& task)
{
    variable_order_finder::VariableOrderFinder vof(
        task,
        variable_order_type,
        rng);
    MergeTreeNode* root = new MergeTreeNode(vof.next());
    while (!vof.done()) {
        MergeTreeNode* right_child = new MergeTreeNode(vof.next());
        root = new MergeTreeNode(root, right_child);
    }
    return std::make_unique<MergeTree>(root, rng, update_option);
}

unique_ptr<MergeTree> MergeTreeFactoryLinear::compute_merge_tree(
    const AbstractTask& task,
    const FactoredTransitionSystem& fts,
    const vector<int>& indices_subset)
{
    /*
      Compute a mapping from state variables to transition system indices
      that contain those variables. Also set all indices not contained in
      indices_subset to "used".
    */
    int num_vars = task.get_variables().size();
    int num_ts = fts.get_size();
    vector<int> var_to_ts_index(num_vars, -1);
    vector<bool> used_ts_indices(num_ts, true);
    for (int ts_index : fts) {
        bool use_ts_index =
            find(indices_subset.begin(), indices_subset.end(), ts_index) !=
            indices_subset.end();
        if (use_ts_index) {
            used_ts_indices[ts_index] = false;
        }
        const vector<int>& vars =
            fts.get_transition_system(ts_index).get_incorporated_variables();
        for (int var : vars) {
            var_to_ts_index[var] = ts_index;
        }
    }

    /*
     Compute the merge tree, using transition systems corresponding to
     variables in order given by the variable order finder, implicitly
     skipping all indices not in indices_subset, because these have been set
     to "used" above.
    */
    variable_order_finder::VariableOrderFinder vof(
        task,
        variable_order_type,
        rng);

    int next_var = vof.next();
    int ts_index = var_to_ts_index[next_var];
    assert(ts_index != -1);
    // find the first valid ts index
    while (used_ts_indices[ts_index]) {
        assert(!vof.done());
        next_var = vof.next();
        ts_index = var_to_ts_index[next_var];
        assert(ts_index != -1);
    }
    used_ts_indices[ts_index] = true;
    MergeTreeNode* root = new MergeTreeNode(ts_index);

    while (!vof.done()) {
        next_var = vof.next();
        ts_index = var_to_ts_index[next_var];
        assert(ts_index != -1);
        if (!used_ts_indices[ts_index]) {
            used_ts_indices[ts_index] = true;
            MergeTreeNode* right_child = new MergeTreeNode(ts_index);
            root = new MergeTreeNode(root, right_child);
        }
    }
    return std::make_unique<MergeTree>(root, rng, update_option);
}

string MergeTreeFactoryLinear::name() const
{
    return "linear";
}

void MergeTreeFactoryLinear::dump_tree_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        dump_variable_order_type(variable_order_type, log);
    }
}

} // namespace merge_and_shrink
