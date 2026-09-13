#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SELECTOR_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SELECTOR_FACTORY_H

#include "probfd/probabilistic_task.h"

#include <memory>
#include <vector>

namespace downward::utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {
class MergeSelector;
}

namespace probfd::merge_and_shrink {

class MergeSelectorFactory {
public:
    virtual ~MergeSelectorFactory() = default;

    virtual std::unique_ptr<MergeSelector>
    compute_selector(const ProbabilisticTaskTuple& task) = 0;

    virtual bool requires_liveness() const = 0;

    virtual bool requires_goal_distances() const = 0;

    void dump_options(downward::utils::LogProxy& log) const;

protected:
    virtual std::string name() const = 0;

    virtual void
    dump_selector_specific_options(downward::utils::LogProxy&) const
    {
    }
};

} // namespace probfd::merge_and_shrink

#endif
