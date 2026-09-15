#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_H

#include "probfd/probabilistic_task.h"

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace downward::merge_and_shrink {
class MergeUpdateStrategy;
class MergeTree;
} // namespace downward::merge_and_shrink

namespace downward::utils {
class LogProxy;
class RandomNumberGenerator;
} // namespace downward::utils

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeTreeFactory {
protected:
    std::shared_ptr<downward::merge_and_shrink::MergeUpdateStrategy>
        merge_update_strategy;

public:
    explicit MergeTreeFactory(
        std::shared_ptr<downward::merge_and_shrink::MergeUpdateStrategy>
            merge_update_strategy);

    virtual ~MergeTreeFactory() = default;

    void dump_options(downward::utils::LogProxy& log) const;

    // Compute a merge tree for the given entire task.
    virtual std::unique_ptr<downward::merge_and_shrink::MergeTree>
    compute_merge_tree(const SharedProbabilisticTask& task) = 0;

    /* Compute a merge tree for the given current factored transition,
       system, possibly for a subset of indices. */
    virtual std::unique_ptr<downward::merge_and_shrink::MergeTree>
    compute_merge_tree(
        const SharedProbabilisticTask& task,
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset);

    virtual bool requires_liveness() const = 0;
    virtual bool requires_goal_distances() const = 0;

protected:
    virtual std::string name() const = 0;

    virtual void dump_tree_specific_options(downward::utils::LogProxy&) const
    {
    }
};

} // namespace probfd::merge_and_shrink

#endif
