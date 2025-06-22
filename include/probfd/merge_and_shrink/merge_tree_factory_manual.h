#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_MANUAL_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_MANUAL_H

#include "probfd/merge_and_shrink/merge_tree_factory.h"

namespace probfd::merge_and_shrink {

class MergeTreeFactoryManual : public MergeTreeFactory {
    int random_seed;
    UpdateOption update_option;
    std::vector<int> linear_order;

public:
    MergeTreeFactoryManual(
        int random_seed,
        UpdateOption update_option,
        std::vector<int> linear_order);

    std::unique_ptr<MergeTree>
    compute_merge_tree(const SharedProbabilisticTask& task) override;

    std::unique_ptr<MergeTree> compute_merge_tree(
        const SharedProbabilisticTask& task,
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset) override;

    bool requires_liveness() const override { return true; }

    bool requires_goal_distances() const override { return false; }

protected:
    std::string name() const override;
    void
    dump_tree_specific_options(downward::utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
