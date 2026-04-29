#ifndef PROBFD_MERGE_AND_SHRINK_MERGE_SELECTOR_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_MERGE_SELECTOR_FACTORY_H

#include "probfd/probabilistic_task.h"

#include <string>

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
    create(SharedProbabilisticTask task) = 0;

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
