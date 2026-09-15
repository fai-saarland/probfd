#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H

#include "probfd/merge_and_shrink/merge_tree_factory.h"

#include "downward/task_utils/variable_order.h"

namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd::merge_and_shrink {
class TaskVariableOrderFactory;
}

namespace probfd::merge_and_shrink {

class MergeTreeFactoryLinear : public MergeTreeFactory {
    std::shared_ptr<TaskVariableOrderFactory> variable_order_factory;

public:
    MergeTreeFactoryLinear(
        std::shared_ptr<downward::merge_and_shrink::MergeUpdateStrategy>
            merge_update_strategy,
        std::shared_ptr<TaskVariableOrderFactory> variable_order_factory);

    std::unique_ptr<downward::merge_and_shrink::MergeTree>
    compute_merge_tree(const SharedProbabilisticTask& task) override;

    std::unique_ptr<downward::merge_and_shrink::MergeTree> compute_merge_tree(
        const SharedProbabilisticTask& task,
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset) override;

    bool requires_liveness() const override
    {
        return false;
    }

    bool requires_goal_distances() const override
    {
        return false;
    }

protected:
    std::string name() const override;

    void
    dump_tree_specific_options(downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
