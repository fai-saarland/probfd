#ifndef PROBFD_MERGE_AND_SHRINK_LABEL_REDUCTION_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_LABEL_REDUCTION_FACTORY_H

#include "probfd/task_dependent_factory.h"

namespace probfd::merge_and_shrink {
class LabelReduction;
}

namespace probfd::merge_and_shrink {

using LabelReductionFactory = TaskDependentFactory<LabelReduction>;

} // namespace probfd::merge_and_shrink

#endif
