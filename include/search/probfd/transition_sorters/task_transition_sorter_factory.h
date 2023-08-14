#ifndef PROBFD_TRANSITION_SORTERS_TASK_SUCCESSOR_SORTER_FACTORY_H
#define PROBFD_TRANSITION_SORTERS_TASK_SUCCESSOR_SORTER_FACTORY_H

#include "probfd/engine_interfaces/fdr_types.h"

#include "probfd/fdr_types.h"

#include <memory>

namespace probfd {

/// Factory interface for successor sorters.
class FDRTransitionSorterFactory {
public:
    virtual ~FDRTransitionSorterFactory() = default;

    /// Creates a successor sorter from a given state and action id map.
    virtual std::shared_ptr<FDRTransitionSorter>
    create_transition_sorter(FDRMDP* state_space) = 0;
};

} // namespace probfd

#endif