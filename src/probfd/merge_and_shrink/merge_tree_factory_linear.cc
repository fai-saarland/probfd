#include "probfd/merge_and_shrink/merge_tree_factory_linear.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_tree.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/tasks/determinization_task.h"

#include <algorithm>

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeTreeFactoryLinear::MergeTreeFactoryLinear(
    int random_seed,
    UpdateOption update_option,
    variable_order_finder::VariableOrderType variable_order)
    : MergeTreeFactory(random_seed, update_option)
    , variable_order_type(variable_order)
{
}

unique_ptr<MergeTree> MergeTreeFactoryLinear::compute_merge_tree(
    const SharedProbabilisticTask& task)
{
    const auto determinization = tasks::create_determinization_task(task);

    variable_order_finder::VariableOrderFinder vof(
        to_refs(determinization),
        variable_order_type,
        rng);

    auto* root = new MergeTreeNode(vof.next());

    while (!vof.done()) {
        auto* right_child = new MergeTreeNode(vof.next());
        root = new MergeTreeNode(root, right_child);
    }

    return std::make_unique<MergeTree>(root, rng, update_option);
}

unique_ptr<MergeTree> MergeTreeFactoryLinear::compute_merge_tree(
    const SharedProbabilisticTask& task,
    const FactoredTransitionSystem& fts,
    const vector<int>& indices_subset)
{
    const auto determinization = tasks::create_determinization_task(task);

    const auto& variables = get_variables(determinization);
    /*
      Compute a mapping from state variables to transition system indices
      that contain those variables. Also set all indices not contained in
      indices_subset to "used".
    */
    const int num_vars = variables.size();
    const int num_ts = fts.get_size();

    vector var_to_ts_index(num_vars, -1);
    vector used_ts_indices(num_ts, true);

    for (int ts_index : fts) {
        used_ts_indices[ts_index] = !ranges::contains(indices_subset, ts_index);

        const vector<int>& vars =
            fts.get_transition_system(ts_index).get_incorporated_variables();

        for (const int var : vars) {
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
        to_refs(determinization),
        variable_order_type,
        rng);

    int next_var;
    int ts_index;

    // find the first valid ts index
    do {
        assert(!vof.done());
        next_var = vof.next();
        ts_index = var_to_ts_index[next_var];
        assert(ts_index != -1);
    } while (used_ts_indices[ts_index]);

    used_ts_indices[ts_index] = true;
    auto* root = new MergeTreeNode(ts_index);

    while (!vof.done()) {
        next_var = vof.next();
        ts_index = var_to_ts_index[next_var];
        assert(ts_index != -1);
        if (!used_ts_indices[ts_index]) {
            used_ts_indices[ts_index] = true;
            auto* right_child = new MergeTreeNode(ts_index);
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

} // namespace probfd::merge_and_shrink