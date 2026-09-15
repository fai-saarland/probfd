#ifndef MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H
#define MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H

#include "downward/merge_and_shrink/merge_tree_factory.h"

namespace downward::variable_order {
class TaskVariableOrderFactory;
}

namespace downward::utils {
class RandomNumberGenerator;
}

namespace downward::merge_and_shrink {
class MergeTreeFactoryLinear : public MergeTreeFactory {
    std::shared_ptr<variable_order::TaskVariableOrderFactory>
        variable_order_factory;

protected:
    std::string name() const override;
    void dump_tree_specific_options(utils::LogProxy& log) const override;

public:
    MergeTreeFactoryLinear(
        std::shared_ptr<variable_order::TaskVariableOrderFactory>
            variable_order_factory,
        std::shared_ptr<MergeUpdateStrategy> merge_update_strategy);

    std::unique_ptr<MergeTree>
    compute_merge_tree(const AbstractTaskTuple& task) override;

    std::unique_ptr<MergeTree> compute_merge_tree(
        const AbstractTaskTuple& task,
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset) override;

    bool requires_init_distances() const override
    {
        return false;
    }

    bool requires_goal_distances() const override
    {
        return false;
    }
};
} // namespace downward::merge_and_shrink

#endif
