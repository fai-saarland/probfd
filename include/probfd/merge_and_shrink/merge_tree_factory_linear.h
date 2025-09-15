#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H

#include "probfd/merge_and_shrink/merge_tree_factory.h"

#include "downward/task_utils/variable_order_finder.h"

namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd::merge_and_shrink {

class MergeTreeFactoryLinear : public MergeTreeFactory {
    downward::variable_order_finder::VariableOrderType variable_order_type;
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng;

public:
    MergeTreeFactoryLinear(
        int random_seed,
        UpdateOption update_option,
        downward::variable_order_finder::VariableOrderType variable_order);

    std::unique_ptr<MergeTree>
    compute_merge_tree(const SharedProbabilisticTask& task) override;

    std::unique_ptr<MergeTree> compute_merge_tree(
        const SharedProbabilisticTask& task,
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset) override;

    bool requires_liveness() const override { return false; }

    bool requires_goal_distances() const override { return false; }

protected:
    std::string name() const override;
    void
    dump_tree_specific_options(downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
