#ifndef MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H
#define MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H

#include "downward/merge_and_shrink/merge_tree_factory.h"

#include "downward/task_utils/variable_order_finder.h"

namespace downward::utils {
class RandomNumberGenerator;
}

namespace downward::merge_and_shrink {
class MergeTreeFactoryLinear : public MergeTreeFactory {
    variable_order_finder::VariableOrderType variable_order_type;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

protected:
    std::string name() const override;
    void dump_tree_specific_options(utils::LogProxy& log) const override;

public:
    MergeTreeFactoryLinear(
        variable_order_finder::VariableOrderType variable_order,
        int random_seed,
        UpdateOption update_option);

    std::unique_ptr<MergeTree>
    compute_merge_tree(const AbstractTaskTuple& task) override;

    std::unique_ptr<MergeTree> compute_merge_tree(
        const AbstractTaskTuple& task,
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset) override;

    bool requires_init_distances() const override { return false; }

    bool requires_goal_distances() const override { return false; }
};
} // namespace downward::merge_and_shrink

#endif
