#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_H

#include <memory>
#include <string>
#include <vector>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace utils {
class LogProxy;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
class ProbabilisticTask;
}

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
class MergeTree;
enum class UpdateOption;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

class MergeTreeFactory {
protected:
    std::shared_ptr<utils::RandomNumberGenerator> rng;
    UpdateOption update_option;

    virtual std::string name() const = 0;
    virtual void dump_tree_specific_options(utils::LogProxy&) const {}

public:
    explicit MergeTreeFactory(const plugins::Options& options);
    virtual ~MergeTreeFactory() = default;

    void dump_options(utils::LogProxy& log) const;

    // Compute a merge tree for the given entire task.
    virtual std::unique_ptr<MergeTree>
    compute_merge_tree(std::shared_ptr<ProbabilisticTask>& task) = 0;

    /* Compute a merge tree for the given current factored transition,
       system, possibly for a subset of indices. */
    virtual std::unique_ptr<MergeTree> compute_merge_tree(
        std::shared_ptr<ProbabilisticTask>& task,
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset);

    virtual bool requires_init_distances() const = 0;
    virtual bool requires_goal_distances() const = 0;

    // Derived classes must call this method in their parsing methods.
    static void add_options_to_feature(plugins::Feature& feature);
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_H
