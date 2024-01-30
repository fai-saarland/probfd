#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_SELECTOR_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_SELECTOR_H

#include <string>
#include <vector>

namespace utils {
class LogProxy;
}

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::merge_and_shrink {
class FactoredTransitionSystem;
}

namespace probfd::merge_and_shrink {

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

    virtual void initialize(const ProbabilisticTaskProxy& task_proxy) = 0;

    void dump_options(utils::LogProxy& log) const;

    virtual bool requires_liveness() const = 0;
    virtual bool requires_goal_distances() const = 0;
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_MERGE_SELECTOR_H
