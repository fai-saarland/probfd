#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SELECTOR_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SELECTOR_H

#include "probfd/probabilistic_task.h"

#include <string>
#include <vector>

namespace downward::utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
}

namespace probfd::merge_and_shrink {

class MergeSelector {
public:
    virtual ~MergeSelector() = default;

    virtual std::pair<int, int> select_merge(
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset = std::vector<int>()) const = 0;

    virtual bool requires_liveness() const = 0;
    virtual bool requires_goal_distances() const = 0;

protected:
    static std::vector<std::pair<int, int>> compute_merge_candidates(
        const FactoredTransitionSystem& fts,
        const std::vector<int>& indices_subset);
};

} // namespace probfd::merge_and_shrink

#endif
