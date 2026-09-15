#include "downward/merge_and_shrink/merge_tree_factory_linear.h"

#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_tree.h"
#include "downward/merge_and_shrink/transition_system.h"

#include "downward/task_utils/task_variable_order_factory.h"
#include "downward/task_utils/variable_order.h"

#include "downward/abstract_task.h"
#include "downward/state.h"

#include "downward/utils/rng.h"

#include <algorithm>

using namespace std;

namespace downward::merge_and_shrink {
MergeTreeFactoryLinear::MergeTreeFactoryLinear(
    std::shared_ptr<variable_order::TaskVariableOrderFactory>
        variable_order_factory,
    std::shared_ptr<MergeUpdateStrategy> merge_update_strategy)
    : MergeTreeFactory(std::move(merge_update_strategy))
    , variable_order_factory(std::move(variable_order_factory))
{
}

unique_ptr<MergeTree>
MergeTreeFactoryLinear::compute_merge_tree(const AbstractTaskTuple& task)
{
    variable_order::VariableOrder vof =
        variable_order_factory->create_variable_order(task);

    auto* root = new MergeTreeNode(vof.next());
    while (!vof.done()) {
        root = new MergeTreeNode(root, new MergeTreeNode(vof.next()));
    }
    return std::make_unique<MergeTree>(root, merge_update_strategy);
}

unique_ptr<MergeTree> MergeTreeFactoryLinear::compute_merge_tree(
    const AbstractTaskTuple& task,
    const FactoredTransitionSystem& fts,
    const vector<int>& indices_subset)
{
    const auto& variables = get_variables(task);

    /*
      Compute a mapping from state variables to transition system indices
      that contain those variables. Also set all indices not contained in
      indices_subset to "used".
    */
    int num_vars = variables.size();
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
    variable_order::VariableOrder vof =
        variable_order_factory->create_variable_order(task);

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
    auto* root = new MergeTreeNode(ts_index);

    while (!vof.done()) {
        next_var = vof.next();
        ts_index = var_to_ts_index[next_var];
        assert(ts_index != -1);
        if (!used_ts_indices[ts_index]) {
            used_ts_indices[ts_index] = true;
            root = new MergeTreeNode(root, new MergeTreeNode(ts_index));
        }
    }
    return std::make_unique<MergeTree>(root, merge_update_strategy);
}

string MergeTreeFactoryLinear::name() const
{
    return "linear";
}

void MergeTreeFactoryLinear::dump_tree_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        variable_order_factory->dump_options(log);
    }
}

} // namespace downward::merge_and_shrink
