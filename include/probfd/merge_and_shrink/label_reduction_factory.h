#ifndef PROBFD_MERGE_AND_SHRINK_LABEL_REDUCTION_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_LABEL_REDUCTION_FACTORY_H

#include "probfd/task_dependent_factory.h"

namespace downward::utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {
class LabelReduction;
}

namespace probfd::merge_and_shrink {

class LabelReductionFactory : public TaskDependentFactory<LabelReduction> {
public:
    virtual void dump_options(downward::utils::LogProxy& log) const = 0;
};

} // namespace probfd::merge_and_shrink

#endif
