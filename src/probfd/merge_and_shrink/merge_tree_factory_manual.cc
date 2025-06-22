#include "probfd/merge_and_shrink/merge_tree_factory_manual.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_tree.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/variable_space.h"

#include <algorithm>
#include <ranges>

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeTreeFactoryManual::MergeTreeFactoryManual(
    int random_seed,
    UpdateOption update_option,
    std::vector<int> linear_order)
    : MergeTreeFactory(random_seed, update_option)
    , linear_order(std::move(linear_order))
{
}

unique_ptr<MergeTree>
MergeTreeFactoryManual::compute_merge_tree(const SharedProbabilisticTask& task)
{
    auto* root = new MergeTreeNode(linear_order.front());

    for (int var : linear_order | std::views::drop(1)) {
        auto* left_child = new MergeTreeNode(var);
        root = new MergeTreeNode(left_child, root);
    }

    return std::make_unique<MergeTree>(root, rng, update_option);
}

unique_ptr<MergeTree> MergeTreeFactoryManual::compute_merge_tree(
    const SharedProbabilisticTask& task,
    const FactoredTransitionSystem& fts,
    const vector<int>& indices_subset)
{
    const auto& variables = get_variables(task);
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

        for (const int var : vars) { var_to_ts_index[var] = ts_index; }
    }

    /*
     Compute the merge tree, using transition systems corresponding to
     variables in order given by the variable order finder, implicitly
     skipping all indices not in indices_subset, because these have been set
     to "used" above.
    */
    int next_var;
    int ts_index;
    auto it = linear_order.begin();

    // find the first valid ts index
    do {
        next_var = *it++;
        ts_index = var_to_ts_index[next_var];
        assert(ts_index != -1);
    } while (used_ts_indices[ts_index]);

    used_ts_indices[ts_index] = true;
    auto* root = new MergeTreeNode(ts_index);

    while (it != linear_order.end()) {
        next_var = *it++;
        ts_index = var_to_ts_index[next_var];
        assert(ts_index != -1);
        if (!used_ts_indices[ts_index]) {
            used_ts_indices[ts_index] = true;
            auto* left_child = new MergeTreeNode(ts_index);
            root = new MergeTreeNode(left_child, root);
        }
    }

    return std::make_unique<MergeTree>(root, rng, update_option);
}

string MergeTreeFactoryManual::name() const
{
    return "manual";
}

void MergeTreeFactoryManual::dump_tree_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log << "[" << linear_order.front();
        for (int var : linear_order | std::views::drop(1)) {
            log << ", " << var;
        }
        log << "]" << std::endl;
    }
}

} // namespace probfd::merge_and_shrink