#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H

#include "probfd/merge_and_shrink/merge_tree_factory.h"

#include "downward/task_utils/variable_order_finder.h"

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
class ProbabilisticTask;
}

namespace probfd::merge_and_shrink {

class MergeTreeFactoryLinear : public MergeTreeFactory {
    variable_order_finder::VariableOrderType variable_order_type;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

protected:
    std::string name() const override;
    void dump_tree_specific_options(utils::LogProxy& log) const override;

public:
    explicit MergeTreeFactoryLinear(const plugins::Options& options);

    std::unique_ptr<MergeTree>
    compute_merge_tree(std::shared_ptr<ProbabilisticTask>& task) override;

    std::unique_ptr<MergeTree> compute_merge_tree(
        std::shared_ptr<ProbabilisticTask>& task,
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset) override;

    bool requires_liveness() const override { return false; }
    bool requires_goal_distances() const override { return false; }

    static void add_options_to_feature(plugins::Feature& feature);
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_LINEAR_H
