#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_H

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace downward::cli::plugins {
class Options;
class Feature;
} // namespace downward::cli::plugins

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

public:
    MergeTreeFactory(int random_seed, UpdateOption update_option);

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

protected:
    virtual std::string name() const = 0;

    virtual void dump_tree_specific_options(utils::LogProxy&) const {}
};

extern void add_merge_tree_factory_options_to_feature(
    downward::cli::plugins::Feature& feature);

extern std::tuple<int, UpdateOption> get_merge_tree_factory_args_from_options(
    const downward::cli::plugins::Options& options);

} // namespace probfd::merge_and_shrink

#endif
