#ifndef MERGE_AND_SHRINK_MERGE_SELECTOR_H
#define MERGE_AND_SHRINK_MERGE_SELECTOR_H

#include "downward/abstract_task.h"

#include <string>
#include <vector>

namespace downward::utils {
class LogProxy;
}

namespace downward::merge_and_shrink {
class FactoredTransitionSystem;

class MergeSelector {
protected:
    virtual std::string name() const = 0;

    virtual void dump_selector_specific_options(utils::LogProxy&) const {}

    std::vector<std::pair<int, int>> compute_merge_candidates(
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset) const;

public:
    MergeSelector() = default;
    virtual ~MergeSelector() = default;
    virtual std::pair<int, int> select_merge(
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset = std::vector<int>()) const = 0;
    virtual void initialize(const AbstractTaskTuple& task) = 0;
    void dump_options(utils::LogProxy& log) const;
    virtual bool requires_init_distances() const = 0;
    virtual bool requires_goal_distances() const = 0;
};
} // namespace downward::merge_and_shrink

#endif
